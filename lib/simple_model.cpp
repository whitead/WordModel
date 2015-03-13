#include "simple_model.hpp"

using namespace std;

wordmodel::SimpleModel::SimpleModel() {
  setup_predict(input_stream_);
}

wordmodel::SimpleModel::SimpleModel(SimpleModel&& sm) :
  parser_(std::move(sm.parser_)),
  word_modes_(std::move(sm.word_modes_)) {
  setup_predict(input_stream_);
}

wordmodel::SimpleModel::~SimpleModel() {
  if(prediction_tok_ != NULL)
    delete prediction_tok_;
}

void wordmodel::SimpleModel::write_summary(ostream& out) {
  out << "Simple Word Model" << endl;
}

bool wordmodel::SimpleModel::putc(char c) {
  input_stream_ << c;
  return true;
}

const std::string& wordmodel::SimpleModel::get_prediction(int* prediction_id) {

  //train
  int pos = input_stream_.tellg();
  do_train(input_stream_);
  input_stream_.seekg(pos);
  
  //just eat up the whole stream
  for(auto t: *prediction_tok_) {   
    prediction_ = word_modes_[t];
  }  
    
  return prediction_;
}


void wordmodel::SimpleModel::do_train(istream& data) {  
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

void wordmodel::SimpleModel::setup_predict(istream& in) {
 
  using namespace boost;
  using namespace std;
  
  //turn the stream into an iterator
  istreambuf_iterator<char> stream_iter(in);
  istreambuf_iterator<char> end_of_stream;

    //build a tokenizer
  char_separator<char> sep(TOKENIZER_DELIMS);
  prediction_tok_ =  new boost::tokenizer<boost::char_separator<char>, std::istreambuf_iterator<char> >(stream_iter,  end_of_stream, sep);

}
