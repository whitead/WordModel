#include "parser.hpp"
#include "tokenizer.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>

using namespace std;

wordmodel::Parser::Parser() : word_number(0) {
  add_word("");
}

bool wordmodel::Parser::get_index(string const& word, size_t* index) const {
  auto it = word_map.find(word);
  if(it == word_map.end())
    return false;
  *index = it->second;
  return true;
}

bool wordmodel::Parser::get_word(size_t index, string* word) const {
  if(index > word_number)
    return false;
  *word = words[index];
  return true;
}

unsigned int wordmodel::Parser::count(string const& word) const {
  //need to make sure this word exists
  size_t index;
  if(get_index(word, &index))
    return counts[index];
  return 0;
}

unsigned int wordmodel::Parser::count(size_t index) const {
  return counts[index];
}

unsigned int wordmodel::Parser::count(size_t index_i, size_t index_j) const {
    auto it = pair_counts.find(wordmodel::Pair_Key(index_i, index_j));  
    if(it != pair_counts.end())
      return it->second;
    return 0;

}
unsigned int wordmodel::Parser::count(string const& word_i, string const& word_j) const {
  //need to make sure this word exists
  auto it1 = word_map.find(word_i);
  auto it2 = word_map.find(word_j);
  if(it1 != word_map.end() && 
     it2 != word_map.end()) {
    auto it3 = pair_counts.find(wordmodel::Pair_Key(it1->second, it2->second));  
    if(it3 != pair_counts.end())
      return it3->second;
  }
  return 0;
}

const vector<size_t>& wordmodel::Parser::iterate_pairs(size_t index) const {
  return nonzero_pairs[index];
}

const vector<size_t>& wordmodel::Parser:: iterate_pairs(string word) const {
  size_t index;
  if(get_index(word, &index))
    return nonzero_pairs[index];
  throw std::out_of_range("Invalid word");
}


void wordmodel::Parser::print_summary(ostream& out) const {
  out << "Word number: " << word_number << endl;

  vector<size_t> top_words;
  unsigned int top_word_number = 20;
  size_t i,j,k;
  
  for(i = 0; i < word_number; ++i) {

    //the boolean condition there relies on a wrap around of the integer
    for(j = top_words.size(); j > 0 && counts[i] > counts[top_words[j-1]]; --j);

    //do insertion
    //grow if possibe
    if(j == top_words.size() && top_words.size() < top_word_number)  {
	top_words.push_back(i);
    }
    else {// knock element off
      //keep element if possible
      if(top_words.size() < top_word_number)
	top_words.push_back(top_words[top_words.size() - 1]);
      for(k = top_words.size() - 1; k > j; --k) {
	top_words[k] = top_words[k - 1];
      }
      top_words[j] = i;
    }
  }

  out << "Top words:" << endl;
  for(i = 0; i < top_words.size(); ++i) {
    out << "\t <" << words[top_words[i]] << "> " << counts[top_words[i]] << endl;
  }
    
}

void wordmodel::Parser::add_word(const std::string& word) {
  last_index_ = add_pair(word, last_index_);
}

size_t wordmodel::Parser::add_pair(const std::string& word, size_t last_index) {

  unordered_map<wordmodel::Pair_Key, unsigned int, wordmodel::Pair_Key_Hasher>::iterator pair_it;
  auto pair_it_end = pair_counts.end();
  auto wm_end = word_map.end();  
  size_t index; 
  wordmodel::Pair_Key key;

  //check if the word is new
  if(word_map.find(word) == wm_end) {
    index = static_cast<size_t>(word_number);      
    word_number++;
    
    //check if we have too many words for the pair_counts
    if(word_number == (numeric_limits<size_t>::max()) / 2) {
      cout << "WARNING! Exceeded maximum pair number (" << word_number << "). Hash collisions may occur" << endl;
    }
    
    word_map[word] = index;
    words.push_back(word);
    counts.push_back(0);      
    nonzero_pairs.push_back(vector<size_t>(0));
    
#ifdef DEBUG_PARSER
    cout << "New word <" << word
	 << ">, at index " << index
	 << endl;
#endif //DEBUG_PARSER
    
  } else
    index = word_map[word];
  
  //increment counts
  counts[index]++;
  
  //check for existance in pair_counts
  if(word_number > 1) {
    key = wordmodel::Pair_Key(last_index, index);
    pair_it = pair_counts.find(key);
    //modify them
    if(pair_it == pair_it_end)
      pair_counts[key] = 1;
    else
      pair_it->second += 1;        
    
    //add to nonzero_pair list
    nonzero_pairs[last_index].push_back(index);
    
#ifdef DEBUG_PARSER
    cout << "Tokens: <" << word << "> <" << words[last_index] << ">"
	 << " Index: " << index
	 <<" Counts: " << counts[index]
      	 <<" Pair: " << pair_counts[key]
	 << endl;
#endif //DEBUG_PARSER
  }

  return index;
}


