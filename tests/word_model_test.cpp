#include "brown_word_model.hpp"
#include "simple_model.hpp"
#include "parser.hpp"
#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE WordModel
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace wordmodel;
using namespace std;
using namespace boost;

struct ParserTest {
  ParserTest(){
    stringstream parsing_string("Hello. I\'m a string! Can you count me\? I Can. I'm done!");
    parser.parse(parsing_string);    
  }  

  Parser parser;

};

BOOST_FIXTURE_TEST_SUITE( parser_test, ParserTest )

BOOST_AUTO_TEST_CASE( parser_single_counts )
{

  BOOST_REQUIRE( parser.count("Hello") == 1 );
  BOOST_REQUIRE( parser.count("H") == 0 );
  BOOST_REQUIRE( parser.count("Can") == 2 );
  BOOST_REQUIRE( parser.count("I\'m") == 2 );
}

BOOST_AUTO_TEST_CASE( parser_delim_counts )
{

  BOOST_REQUIRE( parser.count("!") == 2 );
  BOOST_REQUIRE( parser.count(".") == 2 );
  BOOST_REQUIRE( parser.count("\'") == 0 );
}

BOOST_AUTO_TEST_CASE( parser_pair_counts )
{

  BOOST_REQUIRE( parser.count("a", "string") == 1 );
  BOOST_REQUIRE( parser.count(".", "I\'m") == 2 );
  BOOST_REQUIRE( parser.count("4", "5") == 0 );
  
  size_t index;
  BOOST_REQUIRE( parser.get_index("I\'m", &index) );
  size_t i;
  auto vec = parser.iterate_pairs(".");
  for(auto ii : vec) {
    if(ii == index) {
      i = ii;
      break;
    }
  }
  BOOST_REQUIRE(i == index);
}

BOOST_AUTO_TEST_CASE( parser_timing )
{

  int steps = 100000;
  stringstream parsing_string;
  for(int i = 0; i < steps; i++) {
    parsing_string << "foo" << i << " ";
  }

  timer t;
  Parser growing_parser(parsing_string);    

  BOOST_REQUIRE( t.elapsed() / steps < 0.00005);

}

BOOST_AUTO_TEST_CASE( parser_index_word_conversion )
{
  string word;
  BOOST_REQUIRE( parser.get_word(0, &word) );
  size_t index;
  BOOST_REQUIRE( parser.get_index(word, &index) );  
  BOOST_REQUIRE( index == 0);  
  BOOST_REQUIRE( !parser.get_index("FAFAFSXFAFA", &index) );  
  BOOST_REQUIRE( !parser.get_word(548434343, &word) );  
}


BOOST_AUTO_TEST_CASE( grimm_book )
{
  
  ifstream grimm;
  grimm.open("grimm.txt");
  BOOST_REQUIRE( grimm.is_open() );

  timer t;
  Parser big_parser(grimm);  
  BOOST_REQUIRE( t.elapsed() < 1);

  BOOST_REQUIRE( big_parser.count(",") == 8906 );
  
}

BOOST_AUTO_TEST_SUITE_END()

struct SimpleModelTest {
  SimpleModelTest(){
    stringstream parsing_string("Hello World!");
    sm.train(parsing_string);    
  }  

  SimpleModel sm;

};

BOOST_FIXTURE_TEST_SUITE( simple_model_test, SimpleModelTest )

BOOST_AUTO_TEST_CASE( simplemodel_predict )
{
  stringstream parsing_string("Hello");
  sm.begin_predict(parsing_string);
  BOOST_REQUIRE( sm.get_prediction().compare("World") == 0 );
  
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE( brown_construction )
{
  BrownWordModel model;
  stringstream summary_string;
  model.write_summary(summary_string);
  BOOST_REQUIRE( starts_with(summary_string.str(), "Brown Word Model") );
}
