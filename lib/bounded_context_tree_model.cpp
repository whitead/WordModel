#include "bounded_context_tree_model.hpp"

wordmodel::BoundedCTModel::BoundedCTModel() : BoundedCTModel(5) {}

wordmodel::BoundedCTModel::~BoundedCTModel() {
  delete prediction_tok_;
}

wordmodel::BoundedCTModel::BoundedCTModel(int bound) : token_number_(0),
						       prediction_(0),
						       ct_(20),
						       bound_(bound),
						       mistakes_(0){
  ct_.set_visitor(this);
  //push null string
  words_.push_back("");
  word_map_[""] = token_number_++;
  root_weights_.push_back(0.);
  setup_inputstream();
}
wordmodel::BoundedCTModel::BoundedCTModel(BoundedCTModel&& other) :
  ct_(std::move(other.ct_)),  
  weights_(std::move(other.weights_)),
  root_weights_(std::move(other.root_weights_)),
  words_(std::move(other.words_)),
  prediction_context_(std::move(other.prediction_context_)),  		      
  word_map_(std::move(other.word_map_)),
  prediction_id_(other.prediction_id_),
  bound_(other.bound_),
  mistakes_(other.mistakes_),
  token_number_(other.token_number_),
  prediction_(other.prediction_){
  ct_.set_visitor(this);
  setup_inputstream();
}


void wordmodel::BoundedCTModel::write_summary(std::ostream& out) { 
  ct_.write_graph(out, words_);
}


void wordmodel::BoundedCTModel::putc(char c) {
  input_stream_ << c;
}

const std::string& wordmodel::BoundedCTModel::get_prediction(int* prediction_id) {

  do_predict();
  if(prediction_id != NULL)
    *prediction_id = prediction_id_;
  return words_[prediction_];
}

void wordmodel::BoundedCTModel::prediction_result(int prediction_id, bool outcome) {
  if(!outcome)
    ct_.regret(prediction_id, bound_);
}

//clear the data
void wordmodel::BoundedCTModel::start_predict(ContextData& data) {
  prediction_ = token_number_; //an unseen token
  //clear data and set to root node
  data.resize(token_number_);
  for(int i = 0; i < token_number_; ++i)
    data[i] = root_weights_[i];
}

//finish the prediction
void wordmodel::BoundedCTModel::finish_predict(ContextData& data) {
  double min = token_number_;
  for(int i = 0; i < token_number_; i++) {
    if(data[i] < min) {
      min = data[i];
      prediction_ = i;
    }      
  }
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

void wordmodel::BoundedCTModel::do_predict(void) {

  for(auto t: *prediction_tok_) {   

    //first add it if it hasn't been seen before        
    auto it = word_map_.find(t);
    if(it == word_map_.end()) {
      word_map_[t] = token_number_++;
      words_.emplace_back(t);      
      root_weights_.push_back(static_cast<double>(mistakes_) / 2); 
    }

    //check if we regret it
    //automatically passes on first pass because both are 0
    if(prediction_ != word_map_[t]) {
      ct_.regret(prediction_id_, bound_);
      mistakes_++;
    }

    prediction_context_.push_back(word_map_[t]);

    while(prediction_context_.size() > bound_)
      prediction_context_.pop_front();

    ct_.predict(prediction_context_, 
		prediction_context_.size(),
		&prediction_id_);
  }
}

void wordmodel::BoundedCTModel::setup_inputstream() {
 
  using namespace boost;
  using namespace std;
  
  //turn the stream into an iterator
  istreambuf_iterator<char> stream_iter(input_stream_);
  istreambuf_iterator<char> end_of_stream;

    //build a tokenizer
  char_separator<char> sep(TOKENIZER_DELIMS);
  prediction_tok_ =  new boost::tokenizer<boost::char_separator<char>, std::istreambuf_iterator<char> >(stream_iter,  end_of_stream, sep);

}
