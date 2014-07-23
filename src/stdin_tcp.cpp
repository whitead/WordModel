#include <iostream>
#include <array>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio;

int main(int argc, char* argv[]) {

  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: stdin_tcp <host> <port>" << std::endl;
      return 1;
    }


    //all asio programs get one
    io_service io_service;
    
    //build a resolver and give it host and portname
    tcp::resolver resolver(io_service);
    auto endpoint_it = resolver.resolve(argv[1], argv[2]);
    
    //connect the io_service socket with the endpoint
    tcp::socket socket(io_service);
    connect(socket, endpoint_iterator);
    
    while(true)
      {
	std::array<char, 128> buf;
	boost::system::error_code error;
	
	size_t len = socket.read_some(boost::asio::buffer(buf), error);
	
	if (error == boost::asio::error::eof)
	  break; // Connection closed cleanly by peer.
	else if (error)
	  throw boost::system::system_error(error); // Some other error.
	
	std::cout.write(buf.data(), len);
      }
  
}

