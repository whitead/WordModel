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
    /**
     * Creates a simple model which uses delimiters that separate words
     */
    SimpleModel();
    /**
     * Allows specification of the delimiters used.
     */
    SimpleModel(const char* delimiters[], int dlength);
    SimpleModel(SimpleModel&&);
    SimpleModel(SimpleModel&) = delete; //disallow copy constructor
    SimpleModel& operator=(const SimpleModel&) = delete; //disallow copying
    SimpleModel& operator=(const SimpleModel&&) = delete; 
    ~SimpleModel();
    void write_summary(std::ostream& out) override;
    bool putc(char c) override;
    const std::string& get_prediction(unsigned int* prediction_id) override;
    double get_prediction_weight(unsigned int* prediction_id) const override;
    using WordModel::get_prediction;
    void interface(bool interface) override;
    bool detected_interface() const override;
  private:
    void do_train();

    
    Parser parser_;
    std::string input_string_;
    std::unordered_map<std::string, std::string> word_modes_;
    std::vector< std::string > delimiters_;
    bool detected_interface_;
    
    std::string prediction_;
    unsigned int last_prediction_id_;
    double last_prediction_weight_;
  };

}

#endif //SIMPLE_MODEL_H
