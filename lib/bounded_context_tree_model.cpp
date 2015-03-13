#include "bounded_context_tree_model.hpp"

wordmodel::BoundedCTModel::BoundedCTModel() : BoundedCTModel(5) {}

wordmodel::BoundedCTModel::~BoundedCTModel() {
}

wordmodel::BoundedCTModel::BoundedCTModel(int bound) : token_number_(0),
						       prediction_(0),
						       ct_(20),
						       bound_(bound),
						       mistakes_(0),
						       at_interface_(false){
  ct_.set_visitor(this);
  //push null string
  words_.push_back("");
  word_map_[""] = token_number_++;
  root_weights_.push_back(1.);  
  //push interface token
  words_.push_back("<<!--INTERFACE--!>>");
  word_map_["<<!--INTERFACE--!>>"] = token_number_++;
  root_weights_.push_back(1.);  
}
wordmodel::BoundedCTModel::BoundedCTModel(BoundedCTModel&& other) :
  ct_(std::move(other.ct_)),  
  weights_(std::move(other.weights_)),
  root_weights_(std::move(other.root_weights_)),
  words_(std::move(other.words_)),
  prediction_context_(std::move(other.prediction_context_)),  		      
  word_map_(std::move(other.word_map_)),
  prediction_id_(other.prediction_id_),
  current_string_(std::move(other.current_string_)),
  bound_(other.bound_),
  mistakes_(other.mistakes_),
  token_number_(other.token_number_),
  prediction_(other.prediction_)
  at_interface_(other.at_interface_){
  ct_.set_visitor(this);

}


void wordmodel::BoundedCTModel::write_summary(std::ostream& out) { 
  ct_.write_graph(out, words_);
}


bool wordmodel::BoundedCTModel::putc(char c) {

  //end of stream for now
  if(c == '\x00')
    return;

  if(c == '\b') { //backspace
    if(current_string_.size() > 0)
      current_string_.erase(current_string_.size() - 1);
  } 

  if(std::isprint(c)) {
    current_string_ += c; 
    interface(false);
  }

}

const std::string& wordmodel::BoundedCTModel::get_prediction(int* prediction_id) {

  if(prediction_id != NULL)
    *prediction_id = prediction_id_;
  return words_[prediction_];
}

bool wordmodel::BoundedCTModel::interface() const {
  return at_interface_;
}

void wordmodel::BoundedCTModel::interface(bool at_interface) const {
  at_interface_ = at_interface;
  if(at_interface_)
    do_predict();
}

void wordmodel::BoundedCTModeel::do_predict() {

    //first add our current string if it hasn't been seen before
    auto it = word_map_.find(current_string_);
    if(it == word_map_.end()) {
      word_map_[current_string_] = token_number_++;
      words_.emplace_back(current_string_);
    }

    //check if we regret it
    //automatically passes on first pass because both are 0
    if(prediction_ != word_map_[current_string_]) {
      mistakes_++;
      ct_.regret(prediction_id_, bound_);
    }

    prediction_context_.push_back(word_map_[current_string_]);

    while(prediction_context_.size() > bound_)
      prediction_context_.pop_front();

    ct_.predict(prediction_context_, 
		prediction_context_.size(),
		&prediction_id_);

    current_string_.clear();

}

void wordmodel::BoundedCTModel::prediction_result(int prediction_id, bool outcome) {
  if(!outcome)
    ct_.regret(prediction_id, bound_);
}

//clear the data
void wordmodel::BoundedCTModel::start_predict(ContextData& data) {
  prediction_ = 0; //empty string
  //clear data and set to root node
#ifdef DEBUG_BCT
  std::cout << "Initial weights: " << std::endl;
#endif
  data.resize(root_weights_.size());
  for(int i = 0; i < root_weights_.size(); ++i) {
    data[i] = root_weights_[i];
#ifdef DEBUG_BCT
    std::cout << "\t <" << words_[i] << ">=" << data[i] << std::endl;
#endif

  }
}

//finish the prediction
/* This method is quite slow. Here's what I've tried:
 * 
 * 1. branch prediction hints (slower)
 * 2. Using iterators instead of [] operator (slower)
 * 3. -Ofast -ftree-vectorize. Increased speed by ~10%
 * 
 * Not sure what else I can for finding the minimum element, it's
 * pretty straight forward. 
 *
 */
void wordmodel::BoundedCTModel::finish_predict(ContextData& data) {

  double min = mistakes_;

#ifdef DEBUG_BCT
  std::cout << "Final weights: " << std::endl;
  for(int i = 0; i < data.size(); i++)
    std::cout << "\t <" << words_[i] << ">=" << data[i] << std::endl;
#endif

  for(int i = 0; i < data.size(); i++) {
    if(data[i] < min) {
      min = data[i];
      prediction_ = i;
    }
  }
  
  //push back any new tokens encountered
  while(root_weights_.size() < token_number_)
    root_weights_.push_back(mistakes_); 

}


//add the weghts
void wordmodel::BoundedCTModel::push_predict(node_size node, 
						int depth,
						ContextData& data) {
  //root node is treated during start_prediction
  if(node != root_node) {
    //sparse vector.
    //subtract 1 since root_node is stored separately
    for(auto w: weights_[node - 1]) {
      //iterating through the weights for the given node
      data[w.first] += w.second;
    }
  }
}

void wordmodel::BoundedCTModel::push_regret(node_size node,
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
      weights_[node - 1][prediction_] = 1;
    else
      (*it).second += 1;    
  }  
}

void wordmodel::BoundedCTModel::add_node(node_size node,
					 int depth,
					 ContextData& data) {  
  std::unordered_map<word_size, double> temp;
  weights_.emplace_back(std::move(temp));
}
