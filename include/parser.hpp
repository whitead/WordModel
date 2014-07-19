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
    
    /** \brief Access the counts for a given word
     *
     * \param word The word
     * 
     * \return The number of times a word was observed
     */
    unsigned int count(std::string const& word) const;


    /** \brief Access counts that word_j followed word_i
     * 
     * \param word_i The first word
     * \param word_j The second word
     * 
     * \return The number of times a word was observed
     */    
    unsigned int count(std::string const& word_i, std::string const& word_j) const;

  private:                   
    unsigned long int word_number;
    std::unordered_map<std::string, size_t> word_map;
    std::vector<unsigned int> counts; 
    //    typedef std::pair<size_t, size_t> pair_key_type;
    std::unordered_map<Pair_Key, unsigned int, Pair_Key_Hasher> pair_counts;

  };

}
