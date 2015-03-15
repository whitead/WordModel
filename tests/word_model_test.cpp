#include "simple_model.hpp"
#include "parser.hpp"
#include "bounded_context_tree_model.hpp"
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

/**************************/

struct SimpleModelTest {
  SimpleModel sm;
};

BOOST_FIXTURE_TEST_SUITE( simple_model_test, SimpleModelTest )

BOOST_AUTO_TEST_CASE( simplemodel_construction )
{
  //test out move constructor
  SimpleModel sm;
  SimpleModel sm2 = SimpleModel(std::move(sm));
}


BOOST_AUTO_TEST_CASE( simplemodel_predict )
{
  string s("Hello World Hello");
  for(char& c: s) {
    if(c == ' ')      
      sm.interface(true);
    else
      sm.putc(c);
  }

  sm.interface(true);
  BOOST_REQUIRE( sm.get_prediction().compare("World") == 0 );
  
}

BOOST_AUTO_TEST_SUITE_END()

/**************************/

BOOST_AUTO_TEST_SUITE( bounded_tree_test )


/*
 * Check to make sure the model actually can capture a 101010 pattern within a reasonable time frame. 
 * A sanity test, no an accuracy test.
 */
BOOST_AUTO_TEST_CASE( mistake_count ) {

  BoundedCTModel bcm;

  int mistakes = 0;
  std::vector<std::string> convert;
  convert.push_back("0");
  convert.push_back("1");
  for(int i = 0; i < 100; i++) {
    bcm.putc(convert[i % 2][0]);
    bcm.interface(true);
    mistakes += bcm.get_prediction().compare(std::string(convert[(i+1) % 2])) ? 1 : 0;
  }

  BOOST_REQUIRE( mistakes < 5);
}



BOOST_AUTO_TEST_CASE( bounded_tree_test_edge_cases ) {

  BoundedCTModel bcm; 
  BOOST_REQUIRE( bcm.get_prediction().compare("") == 0 );
  bcm.prediction_result(-5, true);
  bcm.prediction_result(49304393, true);

  string s("\n !!! ! ! ! !  %$5645 $#!$!!$$!  R$#$#3 $#! ! ! ! ");
  for(char& c: s) {
    if(c == ' ')
      bcm.interface(true);
    else
      bcm.putc(c);
  }
  //  BOOST_REQUIRE( bcm.get_prediction().compare("!") == 0 );
}


BOOST_AUTO_TEST_CASE( bounded_tree_split_test ) {

  BoundedCTModel bcm; 
  BOOST_REQUIRE( bcm.get_prediction().compare("") == 0 );
  bcm.prediction_result(-5, true);
  bcm.prediction_result(49304393, true);

  string s("The quick brown fox jumps over the lazy dog for the love of god I should go to bed. ");
  for(char& c: s)
    bcm.putc(c);
  //std::cout << bcm.get_prediction() << std::endl;
  //bcm.write_summary(std::cout);
}


BOOST_AUTO_TEST_CASE( bounded_tree_test_easy ) {
  
  BoundedCTModel bcm; 
  for(int i = 0; i < 25; i++) {
    string s("Hello ");
    for(char& c: s)
      bcm.putc(c);
    bcm.interface(true);

    s = "World ";
    for(char& c: s)
      bcm.putc(c);
    bcm.interface(true);
  }

  BOOST_REQUIRE( bcm.get_prediction().compare("Hello ") == 0 );

  //check output
  /*
  ifstream graph_ref;
  graph_ref.open("hello_world_ref.dot");

  stringstream ss;
  bcm.write_summary(ss);
  
  while(ss.good() && graph_ref.good()) {
    BOOST_REQUIRE( ss.get() == graph_ref.get() );
  }
  */

}


BOOST_AUTO_TEST_CASE( bounded_tree_grimm ) {

  BoundedCTModel bcm; 
  ifstream grimm;
  grimm.open("grimm.txt");
  BOOST_REQUIRE( grimm.is_open() );

  timer t;
  int length = 10000;
  while(grimm.good() && length > 0) {
    bcm.putc(grimm.get());
    length--;
  }

  std::cout << t.elapsed() << std::endl;
  BOOST_REQUIRE( t.elapsed() < 0.5);
}


BOOST_AUTO_TEST_SUITE_END()

