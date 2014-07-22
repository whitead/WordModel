#ifndef WORD_MODEL_H
#define WORD_MODEL_H

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

    /** \brief Train the model
     *  
     *  \param data The data to fit 
     *  \param reg The regularizer
     */
    virtual void train(std::istream& data) = 0;
    /** \brief Perform the maximization step of the EM-algorithm
     *         with the given regularization (if possible)
     * 
     *  \param data The data to fit
     *  \param cexp The class expectations, usually from e_step
     */ 

    /** \brief Set an input stream which is used for prediction. Prediction
     *  can be obtained with the >> method
     * 
     * \param in The stream used to predict the next token
     */ 
    virtual void begin_predict(std::istream& in) = 0;

    /** \breif Return the currently predicted word
     *
     *
     */
    virtual std::string get_prediction() const = 0;

  };

}

#endif //WORD_MODEL_H
