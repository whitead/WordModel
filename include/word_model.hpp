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


    /** \brief Set an input stream which is used for prediction. Prediction
     *  can be obtained with get_prediction() method.
     *
     * This will simply set the input stream for prediction. The function
     * returns immediately and will consume the stream when get_prediction()
     * is called. 
     * 
     * \param in The stream used to predict the next token
     */ 
    virtual void begin_predict(std::istream& in) = 0;

    /** \breif Return the currently predicted word
     *
     *
     */
    virtual std::string get_prediction() = 0;

  };

}

#endif //WORD_MODEL_H
