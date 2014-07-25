#include "simple_model.hpp"

using namespace std;

wordmodel::SimpleModel::SimpleModel() : prediction_tok_(NULL) {}

wordmodel::SimpleModel::~SimpleModel() {
  if(prediction_tok_ != NULL)
    delete prediction_tok_;
}

void wordmodel::SimpleModel::write_summary(ostream& out) const {
  out << "Simple Word Model" << endl;
}

void wordmodel::SimpleModel::train(istream& data) {  
  //parse
  parser_.parse(data);
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

void wordmodel::SimpleModel::begin_predict(istream& in) {
 
  using namespace boost;
  using namespace std;
  
  //turn the stream into an iterator
  istreambuf_iterator<char> stream_iter(in);
  istreambuf_iterator<char> end_of_stream;

    //build a tokenizer
  char_separator<char> sep(TOKENIZER_DELIMS);
  prediction_tok_ = new tokenizer<char_separator<char>, istreambuf_iterator<char> >(stream_iter,  end_of_stream, sep);

}

std::string wordmodel::SimpleModel::get_prediction() {

  if(prediction_tok_ == NULL)
    return prediction_;

  //just eat up the whole stream
  for(auto t: *prediction_tok_) {
    prediction_ = word_modes_[t];
  }  
  return prediction_;
}
