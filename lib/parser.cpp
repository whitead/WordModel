#include "parser.hpp"
#include "tokenizer.hpp"
#include <algorithm>


using namespace std;

wordmodel::Parser::Parser (istream& data) : word_number(0){
  parse(data);
}

wordmodel::Parser::Parser() : word_number(0) {
}


unsigned int wordmodel::Parser::count(string const& word) const {
  //need to make sure this word exists
  auto it = word_map.find(word);
  if(it != word_map.end())
    return counts[it->second];
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


void wordmodel::Parser::parse(istream& data) {
  using namespace boost;

  //turn the stream into an iterator
  istreambuf_iterator<char> stream_iter(data);
  istreambuf_iterator<char> end_of_stream;

  //build a tokenizer
  char_separator<char> sep(TOKENIZER_DELIMS);
  tokenizer<char_separator<char>, istreambuf_iterator<char> > tok(stream_iter, end_of_stream, sep);

  //need this iterator checking unordered_map return values
  size_t index; 

  //treat the first token out of the loop
  auto tok_it = tok.begin();
  index = static_cast<size_t>(word_number);
  word_number++;
  word_map[(*tok_it)] = index;
  counts.push_back(1);  
  auto last_t = (*tok_it);

#ifdef DEBUG_PARSER    
  cout << "Token: <" << *tok_it
       << "> Index: " << index
       <<" Counts: " << counts[index]
       << endl;
#endif //DEBUG_PARSER


  ++tok_it;
  unordered_map<wordmodel::Pair_Key, unsigned int, wordmodel::Pair_Key_Hasher>::iterator pair_it;
  auto pair_it_end = pair_counts.end();
  auto wm_end = word_map.end();  

  wordmodel::Pair_Key key;

  //convert stream into tokens
  for(;tok_it != tok.end(); ++tok_it) {
    
    auto t = (*tok_it);

    //check if the word is new
    if(word_map.find(t) == wm_end) {
      index = static_cast<size_t>(word_number);
      word_number++;
      word_map[t] = index;
      counts.push_back(0);

#ifdef DEBUG_PARSER
      cout << "New word <" << t
	   << ">, at index " << index
	   << endl;
#endif //DEBUG_PARSER
      
    } else
      index = word_map[t];

    //increment counts
    counts[index]++;
   
    //check for existance in pair_counts
    key = wordmodel::Pair_Key(word_map[last_t], index);
    pair_it = pair_counts.find(key);
    //modify them
    if(pair_it == pair_it_end)
      pair_counts[key] = 1;
    else
      pair_it->second += 1;        

#ifdef DEBUG_PARSER
    cout << "Tokens: <" << t << "> <" << last_t << ">"
	 << " Index: " << index
	 <<" Counts: " << counts[index]
      	 <<" Pair: " << pair_counts[key]
	 << endl;
#endif //DEBUG_PARSER
    
    last_t = t;

  }
}
