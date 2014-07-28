

#ifndef BOUNDED_CONTEXT_TREE_MODEL_H
#define BOUNDED_CONTEXT_TREE_MODEL_H

#include "word_model.hpp"
#include "context_tree.hpp"
#include <ostream>
#include <unordered_map>
#include <istream>
#include <sstream>


namespace wordmodel {
  /** \brief A simple word model that just chooses the most frequently
   * occuring pairs
   *
   */
  class BoundedCTModel : public WordModel {    
    friend class ContextTree; //allows context tree access for callbacks

    typedef size_t node_size; //to discriminate logically between the 2
    typedef size_t word_size;
    
    typedef std::vector<double> ContextData;

    enum {root_node = 0}

  public: 
    BoundedCTModel(int bound);
    BoundedCTModel(BoundedCTModel&) = delete; //disallow copy constructor
    BoundedCTModel& operator=(const BoundedCTModel&) = delete; //disallow copying
    BoundedCTModel& operator=(const BoundedCTModel&&) = delete; 
    ~BoundedCTModel() {}

    void write_summary(std::ostream& out) const override;
    void putc(char c) override;
    const std::string& get_prediction(int* prediction_id) override;
    using WordModel::get_prediction;
  private:
    void start_predict(ContextData& data);
    void push_predict(node_size node, int i, ContextData& data);
    void finish_predict(node_size node, int i, ContextData& data);
    void push_regret(node_size node, int i, ContextData& data);
    void add_node(node_size node, int i, ContextData& data);


    ContextTree<BoundedCTModel, word_size, ContextData>  ct_;
    //dense in the number of nodes, sparse in the words
    std::vector<node_size, std::unordered_map<word_size, double> > weights_;    
    ContextData root_weights_;
    //goes from words to index
    std::unordered_map<std::string, word_size> word_map_;
    //goes from index to words
    std::vector<std::string> words_; 
    //Contains the context for the context tree
    std::queue<word_size> prediction_context_;
    std::string current_string_;
    int prediction_id_;
    word_size token_number_;
    word_size prediction_;
  };

}

#endif //BOUNDED_CONTEXT_TREE_MODEL_H

