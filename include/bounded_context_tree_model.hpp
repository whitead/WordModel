#ifndef BOUNDED_CONTEXT_TREE_MODEL_H
#define BOUNDED_CONTEXT_TREE_MODEL_H

#include "word_model.hpp"
#include "context_tree.hpp"
#include "parser.hpp"
#include <ostream>
#include <deque>
#include <unordered_map>
#include <istream>
#include <sstream>
#include <cmath>


namespace wordmodel {
  /** \brief A simple word model that just chooses the most frequently
   * occuring pairs
   *
   */
  class BoundedCTModel : public WordModel {    
    typedef size_t node_size; //to discriminate logically between the 2
    typedef size_t word_size;
    
    typedef std::vector<double> ContextData;

    //allows context tree access for callbacks
    friend class ContextTree<BoundedCTModel, word_size, ContextData, 10>;

    enum {root_node = 0, interface_id = 1};

  public: 
    BoundedCTModel();
    ~BoundedCTModel();
    BoundedCTModel(unsigned int bound);
    BoundedCTModel(BoundedCTModel&&);
    BoundedCTModel(BoundedCTModel&) = delete; //disallow copy constructor
    BoundedCTModel& operator=(const BoundedCTModel&) = delete; //disallow copying
    BoundedCTModel& operator=(const BoundedCTModel&&) = delete; 

    void write_summary(std::ostream& out) override;
    bool putc(char c) override;
    const std::string& get_prediction(unsigned int* prediction_id) override;
    void prediction_result(unsigned int prediction_id, bool outcome) override;
    using WordModel::get_prediction;
    bool detected_interface() const;   
    long int training_mistakes() const override;   
    void interface(bool interfaces);

  private:    
    void start_predict(ContextData& data);
    void push_predict(node_size node, int i, ContextData& data);
    void finish_predict(ContextData& data);
    void push_regret(node_size node, int i, ContextData& data);
    void push_reinforce(node_size node, int i, ContextData& data);
    void add_node(node_size node, int i, ContextData& data, bool regret);
    void do_predict();

    static const std::string INTERFACE_TOKEN;

    word_size token_number_;
    word_size prediction_;
    ContextTree<BoundedCTModel, word_size, ContextData, 10>  ct_;
    //dense in the number of nodes, sparse in the words
    std::vector<std::unordered_map<word_size, double> > weights_;    
    ContextData root_weights_;
    //goes from words to index
    std::unordered_map<std::string, word_size> word_map_;
    //goes from index to words
    std::vector<std::string> words_; 
    //Contains the context for the context tree
    std::deque<word_size> prediction_context_;
    unsigned int prediction_id_;
    unsigned int bound_;
    long int mistakes_;
    std::string current_string_;
    bool detected_interface_;
  };

}

#endif //BOUNDED_CONTEXT_TREE_MODEL_H

