#include "simple_model.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

using namespace std;

void wordmodel::SimpleModel::write_summary(ostream& out) const {
  out << "Simple Word Model" << endl;
}

void wordmodel::SimpleModel::train(istream& data) {  
  //parse
  wordmodel::Parser parser(data);
  //build word_modes
  unsigned int count;
  std::string word_i, word_j;
  for(size_t i = 0; i < parser.get_word_number(); ++i) {
    count = 0;
    for(auto j: parser.iterate_pairs(i)) {
      if(parser.count(i,j) > count) {
	parser.get_word(i, &word_i);
	parser.get_word(j, &word_j);
	word_modes[word_i] = word_j;
	count = parser.count(i,j);
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
  tokenizer<char_separator<char>, istreambuf_iterator<char> > tok(stream_iter, end_of_stream, sep);

  for(auto t: tok) {
    prediction_lock.lock();
    prediction = word_modes[t];
    prediction_lock.unlock();
  }
}

std::string wordmodel::SimpleModel::get_prediction() const {
  
  prediction_lock.lock();
  std::string result = prediction;
  prediction_lock.unlock();
  return result;
}
