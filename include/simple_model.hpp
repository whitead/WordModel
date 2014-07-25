#ifndef SIMPLE_MODEL_H
#define SIMPLE_MODEL_H

#include "word_model.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include <ostream>
#include <unordered_map>
#include <istream>

namespace wordmodel {
  /** \brief A simple word model that just chooses the most frequently
   * occuring pairs
   *
   */
  class SimpleModel : WordModel {
  public: 
    SimpleModel();
    ~SimpleModel();
    void write_summary(std::ostream& out) const override;
    void train(std::istream& data) override;
    void begin_predict(std::istream& in) override;
    std::string get_prediction() override;
  private:
    Parser parser_;
    std::unordered_map<std::string, std::string> word_modes_;
    std::string prediction_;
    boost::tokenizer<boost::char_separator<char>, std::istreambuf_iterator<char> >* prediction_tok_;
  };

}

#endif //SIMPLE_MODEL_H
