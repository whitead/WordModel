#include <ostream>
#include <istream>
#include "tokenizer.hpp"
#include "regularizer.hpp"

namespace wordmodel {
  /**\brief The interface for a WordModel
   *
   */
  class WordModel {
  public:
    /** \brief Write a summary to string of the model
     *  
     * Notes: also create a sql version of this
     *
     * \param out Output stream to write summary
     */
    virtual void write_summary(std::ostream& out) const = 0;
    /** \brief Return log-likelihood of the given stream 
     *
     * \param data The text with which to calculate log-likelihood 
     * \return The log-likelihood
     */
    virtual double log_likelihood(std::istream& data) const = 0;
    /** \brief Perform the expectation step of the EM-algorithm 
     *         with the given regularization (if possible)
     *  
     *  \param data The data to fit 
     *  \param reg The regularizer
     *  \return The class expectations for use in the m_step
     */
    virtual double* e_step(std::istream& data, Regularizer const& reg) = 0;
    /** \brief Perform the maximization step of the EM-algorithm
     *         with the given regularization (if possible)
     * 
     *  \param data The data to fit
     *  \param cexp The class expectations, usually from e_step
     */ 
    virtual void m_step(std::istream& data, double* cexp, Regularizer const& reg) = 0;
    /** \brief Set an input stream which is used for prediction. Prediction
     *  can be obtained with the >> method
     * 
     * \param in The stream used to predict the next token
     */ 
    virtual void begin_predict(std::istream& in) = 0;

  };

}
