#include "bounded_context_tree_model.hpp"

wormodel::BoundedCTModel::BoundedCTModel(int bound) : token_number_(0),
						      prediction_(0),
						      ct_(20),
				       prediction_context_(bound){
  ct_.set_visitor(this);
}

void wordmodel::BoundedCTModel::write_summary(std::ostream& out) const {
  out << "Bounded Context Tree Model" << std::endl;
}


void wordmodel::BoundedCTModel::putc(char c) {
  if(std::isspace(c)) {

    if(current_string_.length() <= 1)
      return;

    //first add it if it hasn't been seen before        
    auto it = word_map_.find(current_string_);
    if(it == word_map_.end()) {
      word_map_[current_string_] = token_number_++;
      words_.emplace_back(current_string_);
      root_weights_.push_back(0.); //start with 0 weight in root
    }

    //check if we regret it
    //automatically passes on first pass because both are 0
    if(prediction_ != word_map_[current_string_])
      ct_.regret(prediction_id_, prediction_context_.size());

    prediction_context_.push(word_map_[current_string_]);
    current_string_.clear();
    ct_.predict(prediction_context_, prediction_context_.size(),&prediction_id_);
  } else
    current_string_ += c;  
}

const std::string& wordmodel::BoundedCTModel::get_prediction(int* prediction_id) {
  *prediction_id = prediction_id_;
  return words_[prediction_]
}


//clear the data
void wordmodel::BoundedCTModel::start_prediction(ContextData& data) {
  prediction_ = token_number_; //an unseen token
  //clear data and set to root node
  data.resize(token_number_);
  for(int i = 0; i < token_number_; ++i)
    data[i] = root_weights_[i];
}

//finish the prediction
void wordmodel::BoundedCTModel::finish_prediction(ContextData& data) {
  double min = token_number_;
  for(int i = 0; i < data.size(); i++) {
    if(data[i] < min) {
      min = data[i];
      prediction_ = i;
    }      
  }
}


//add the weghts
void wordmodel::BoundedCTModel::push_prediction(node_size node, 
						int depth,
						ContextData& data) {
  //root node is treated during start_prediction
  if(node != root_node) {
    //sparse vector.
    //subtract 1 since root_node is stored separately
    for(auto w: weights_[node - 1]) {
      //iterating through the weights for the given node
      data[(*w).first] += (*w).second;
    }
  }
}

void wordomdel::BoundedTCModel::push_regret(node_size node,
					    int depth,
					    ContextData& data) {

  if(node == root_node) {
    root_weights_[prediction_] += 1;
  } else {  
    //subtract 1 since root_node is stored separately
    auto it = weights_[node - 1].find(prediction_);
  
    //subtract 1 since root_node is stored separately
    //add it
    if(it == weights_[node-1].end())
      (*it).second = 1;
    else
      (*it).second += 1;    
  }  
}

void wordomdel::BoundedTCModel::add_node(node_size node,
					 int depth,
					 ContextData& data) {  
  weights_.push_back()
}
