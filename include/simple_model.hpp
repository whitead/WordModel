#ifndef SIMPLE_MODEL_H
#define SIMPLE_MODEL_H

#include "word_model.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include <ostream>
#include <unordered_map>
#include <istream>
#include <sstream>

namespace wordmodel {
  /** \brief A simple word model that just chooses the most frequently
   * occuring pairs
   *
   */
  class SimpleModel : public WordModel {
  public: 
    SimpleModel();
    SimpleModel(SimpleModel&&);
    SimpleModel(SimpleModel&) = delete; //disallow copy constructor
    SimpleModel& operator=(const SimpleModel&) = delete; //disallow copying
    SimpleModel& operator=(const SimpleModel&&) = delete; 
    ~SimpleModel();
    void write_summary(std::ostream& out) override;
    void putc(char c) override;
    const std::string& get_prediction(int* prediction_id) override;
    using WordModel::get_prediction;
  private:
    void do_train(std::istream& data);
    void setup_predict(std::istream& in);

    Parser parser_;
    std::stringstream input_stream_;
    std::unordered_map<std::string, std::string> word_modes_;
    std::string prediction_;
    boost::tokenizer<boost::char_separator<char>, std::istreambuf_iterator<char> >* prediction_tok_;
  };

}

#endif //SIMPLE_MODEL_H
