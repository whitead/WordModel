#ifndef SIMPLE_MODEL_H
#define SIMPLE_MODEL_H

#include "word_model.hpp"
#include <ostream>
#include <unordered_map>
#include <istream>
#include <mutex>

namespace wordmodel {
  /** \brief A simple word model that just chooses the most frequently
   * occuring pairs
   *
   */
  class SimpleModel : WordModel {
  public: 
    void write_summary(std::ostream& out) const override;
    void train(std::istream& data) override;
    void begin_predict(std::istream& in) override;
    std::string get_prediction() const override;
  private:
    std::unordered_map<std::string, std::string> word_modes;
    std::string prediction;
    mutable std::mutex prediction_lock;
  };

}

#endif //SIMPLE_MODEL_H
