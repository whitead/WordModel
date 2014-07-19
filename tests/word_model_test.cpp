#include "brown_word_model.hpp"
#include "parser.hpp"
#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE WordModel
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>
#include <ctime>

using namespace wordmodel;
using namespace std;
using namespace boost;

//NOTE: Need to create a test suite for parser and then test a few thing with a fixture
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
  cout << t.elapsed() / steps << endl;

  BOOST_REQUIRE( t.elapsed() / steps < 0.00005);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE( brown_construction )
{
  BrownWordModel model;
  stringstream summary_string;
  model.write_summary(summary_string);
  BOOST_REQUIRE( starts_with(summary_string.str(), "Brown Word Model") );
}
