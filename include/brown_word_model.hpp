#include "word_model.hpp"
#include <ostream>
#include <istream>

namespace wordmodel {
  /** \brief A word model based on the Brown et al 1992 paper
   *
   */
  class BrownWordModel : WordModel {
  public: 
    void write_summary(std::ostream& out) const override;
    double log_likelihood(std::istream& data) const override;
    double* e_step(std::istream& data, Regularizer const& reg) override;
    void m_step(std::istream& data, double* cexp, Regularizer const& reg) override;
    void begin_predict(std::istream& in) override;

  };

}
