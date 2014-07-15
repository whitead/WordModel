#include "brown_word_model.hpp"
#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE WordModel
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>

using namespace wordmodel;
using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( brown_construction )
{
  BrownWordModel model;
  stringstream summary_string;
  model.write_summary(summary_string);
  BOOST_REQUIRE( starts_with(summary_string.str(), "Brown Word Model") );
}
