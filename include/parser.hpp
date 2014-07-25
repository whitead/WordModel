#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

namespace wordmodel {

  typedef std::pair<std::size_t, std::size_t> Pair_Key;

  struct Pair_Key_Hasher
  {
    std::size_t operator()(const Pair_Key& k) const
    {
      //The multiplication is necessary so that the 
      // two pairs don't commute ( x,y != y,x)
      return (k.first * 0x1f1f1f1f) ^ k.second;
    }
  };


  /** \brief A parser for converting a tokenizer into information about
   * words.
   */
  class Parser {
  public:

    /** \brief Constructs a parser
     *
     *
     * \param data The stream from which to parse the counts
     * \param class_number The number of classes 
     */
    Parser(std::istream& data);

    /** \brief Default constructor
     *
     *
     */
    Parser();
    
    /** \brief parse data stream
     *
     */
    void parse(std::istream& data);

    /** \brief Get an index for a given word
     * 
     * \param word The word
     * \param index The index will be returned through this pointer
     * \return Success or failure. It can fail if the word has not been encountered by the parser
     */
    bool get_index(std::string const& word, size_t* index) const;

    /** \brief Get the word corresponding to the given index
     * 
     * \param index The index
     * \param word The word will be returned through this pointer
     * \return Success or failure. It can fail if the index is invalid
     */

    bool get_word(size_t index, std::string* word) const;

    
    /** \brief Access the counts for a given word
     *
     * \param word The word
     * 
     * \return The number of times a word was observed
     */
    unsigned int count(std::string const& word) const;

    /** \brief Access the counts for a given word
     *
     * \param index The index of the word
     * 
     * \return The number of times a word was observed
     */
    unsigned int count(size_t index) const;


    /** \brief Access counts that word_j followed word_i
     * 
     * \param word_i The first word
     * \param word_j The second word
     * 
     * \return The number of times a word was observed
     */    
    unsigned int count(std::string const& word_i, std::string const& word_j) const;

    /** \brief Access counts that a word corresponding to index_j
     * followed a word corresponding to index_i
     * 
     * \param index_i The first word index
     * \param index_j The second word index
     * 
     * \return The number of times a word was observed
     */    
    unsigned int count(size_t index_i, size_t index_j) const;


    /** \brief Returns an iterator that iterates words that followed the
     * given word
     *
     */
    const std::vector<size_t>& iterate_pairs(size_t index) const;

    /** \brief Returns an iterator that iterates words that followed the
     * given word
     *
     */
    const std::vector<size_t>& iterate_pairs(std::string word) const;

    
    /** \brief Output summary statistics from the parsing
     *
     * \param out The stream where to print the statistics
     * 
     */
    void print_summary(std::ostream& out) const;
    
    size_t get_word_number() const {
      return word_number;
    };

  private:                   
    size_t word_number;
    //This is the last index from a previous train
    size_t last_index_;
    //goes from words to index
    std::unordered_map<std::string, size_t> word_map;
    //count of words
    std::vector<unsigned int> counts; 
    //goes from index to words
    std::vector<std::string> words; 
    //for speeding computations, all the nonzero-pairs for a given word
    std::vector<std::vector<size_t> > nonzero_pairs; 
    //pair counts
    std::unordered_map<Pair_Key, unsigned int, Pair_Key_Hasher> pair_counts;
    //Note: to add tuples beyond pairs, use the boost hash_combine function
    //to map the size_t indices of the words.     
  };

}

#endif //PARSER_H
