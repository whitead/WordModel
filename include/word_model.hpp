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

    /** \brief Send if a prediction was used or not
     *
     * The prediction id comes from calling the predict method first. 
     *  
     *  \param prediction_id  Then prediction id
     *  \param outcome True if prediction was used
     */
    virtual void prediction_result(int prediction_id, bool outcome) {};

    /** \brief Send a character to the model
     *
     * The will add a character to the model, which is used
     * to prepare the model for a get_prediction() call. Call
     * clear_prediction() to discard all previous characters passed.
     * This will also train the model online.
     *   
     * \param in The stream used to predict the next token
     */ 
    virtual void putc(char c) = 0;
    
    /** \brief Discard all characters passed with putc. 
     *
     */
    virtual void clear_prediction() {}

    /** \breif Return a predicted string
     * 
     * The passed integer will be set to  a prediction_id which can be used
     * for reinforcement training with a call to reinforce()
     * 
     * \param prediction_id corresponding to this prediction
     *
     * \return Prediction
     */
    virtual const std::string& get_prediction(int* prediction_id) = 0;

    /** \breif Return a predicted string
     * 
     *
     * \return Prediction
     */
    const std::string& get_prediction() {return get_prediction(NULL);}

  };

}

#endif //WORD_MODEL_H
