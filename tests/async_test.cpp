#include "packet.hpp"
#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE AsyncTest
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cstring>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( packet_test )
{
  Packet p;
  char* body = p.body();
  const char* string = "I am a sentence";
  std::memcpy(body, string, std::strlen(string));
  p.body_length(std::strlen(string));
  p.type(Packet::PACKET_TYPE::TRAIN);
  int length = p.length();
  p.encode_header();
  BOOST_REQUIRE( p.decode_header() );
  BOOST_REQUIRE( p.length() == length );
  BOOST_REQUIRE( p.body_length() == std::strlen(p.body()) );
  BOOST_REQUIRE( p.type() == Packet::PACKET_TYPE::TRAIN );


}

