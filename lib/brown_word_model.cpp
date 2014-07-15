#include "brown_word_model.hpp"

using namespace std;

void wordmodel::BrownWordModel::write_summary(ostream& out) const {
  out << "Brown Word Model" << endl;
}

double wordmodel::BrownWordModel::log_likelihood(istream& data) const {
  return 0.;
}
double* wordmodel::BrownWordModel::e_step(istream& data, wordmodel::Regularizer const& reg) {  
  return new double[1];
}
void wordmodel::BrownWordModel::m_step(istream& data, double* cexp,  wordmodel::Regularizer const& reg) {

}

void wordmodel::BrownWordModel::begin_predict(istream& in) {
  
}
