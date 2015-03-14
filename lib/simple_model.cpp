#include "simple_model.hpp"

wordmodel::SimpleModel::SimpleModel(){
  std::string delims(TOKENIZER_DELIMS);
  for(int i = 0; i < delims.size(); i++) {
    delimiters_.push_back(delims.substr(i,1));
  }
}

wordmodel::SimpleModel::SimpleModel(const char* delimiters[], int dlength){
  for(int i = 0; i < dlength; i++) {
    delimiters_.push_back(delimiters[i]);
  }
}


wordmodel::SimpleModel::SimpleModel(SimpleModel&& sm) :
  parser_(std::move(sm.parser_)),
  word_modes_(std::move(sm.word_modes_)),
  delimiters_(std::move(sm.delimiters_)){
}

wordmodel::SimpleModel::~SimpleModel() {
}

void wordmodel::SimpleModel::write_summary(std::ostream& out) {
  out << "Simple Word Model:" << std::endl;
  out << "Word : Mode " << std::endl;
  for ( auto it = word_modes_.begin(); it != word_modes_.end(); ++it )
    out << " [" << it->first <<  "] : [" << it->second << "] " << std::endl;
  out << std::endl;
  
}

bool wordmodel::SimpleModel::putc(char c) {

  //end of stream for now
  if(c == '\x00')
    return detected_interface_;
  else if(c == '\b') { //backspace
    if(input_string_.size() > 0) {
      input_string_.erase(input_string_.size() - 1);
    } else
      return detected_interface_;
  } else {
    input_string_ += c;
  }

  detected_interface_ = false;

  //If there are no delimiters, we always return an interface
  if(delimiters_.size() == 0)
    detected_interface_ = true;

  //check over all the delimiters
  for(auto const &d : delimiters_) {
    if(!input_string_.compare(d)) {
      detected_interface_ = true;
      break;
    }
  }

  return detected_interface_;
}

const std::string& wordmodel::SimpleModel::get_prediction(int* prediction_id) {
  
  //assign reference
  if(prediction_id != NULL)
    last_prediction_id_ = *prediction_id;
    
  return prediction_;
}

bool wordmodel::SimpleModel::detected_interface() const {  
  return detected_interface_;
}

void wordmodel::SimpleModel::interface(bool interface) {  
  if(interface && input_string_.size() > 0) {
    parser_.add_word(input_string_);
    do_train();
    prediction_ = word_modes_[input_string_];
    //store weight if needed
    last_prediction_weight_ = parser_.count(input_string_, prediction_);

    //clear the stream
    input_string_.clear();
  }
}

void wordmodel::SimpleModel::do_train() {  
  //build word_modes_
  unsigned int count;
  std::string word_i, word_j;
  for(size_t i = 0; i < parser_.get_word_number(); ++i) {
    count = 0;
    for(auto j: parser_.iterate_pairs(i)) {
      if(parser_.count(i,j) > count) {
	parser_.get_word(i, &word_i);
	parser_.get_word(j, &word_j);
	word_modes_[word_i] = word_j;
	count = parser_.count(i,j);
      }
    }
  }
}

double wordmodel::SimpleModel::get_prediction_weight(int* prediction_id) const {
  if(*prediction_id == last_prediction_id_)
    return last_prediction_weight_;
  return 0;
}

