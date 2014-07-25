#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <locale>
#include <termios.h>
#include <boost/asio.hpp>
#include "packet.hpp"
#include "service_client.hpp"



using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {

  try  {
    if (argc != 3)
      {
	std::cerr << "Usage: stdin_tcp <host> <port or service>" << std::endl;
	return 1;
      }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
    ServiceClient client(io_service, endpoint_iterator);

    std::thread t([&io_service](){ io_service.run(); });


    // Black magic to prevent Linux from buffering keystrokes.
    struct termios tios;
    tcgetattr(STDIN_FILENO, &tios);
    tios.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &tios);


    //we read character by character, sending packets when a space 
    //is reached
    char word[Packet::max_body_length + 1];
    char c;
    size_t read_count = 0;
    while (!std::cin.eof()) {
      c = std::cin.get();
      if(std::isspace(c)) {
	if(read_count > 0) {
	  //ok, non-empty word is complete. Send packet
	  word[read_count++] = c; //add space, so tokenizer on the other side treats it correctly
	  
	  word[read_count++] = '\0';
	  Packet p;
	  p.type(Packet::PACKET_TYPE::PREDICT);
	  p.body_length(read_count);
	  std::memcpy(p.body(), word, p.body_length());
	  p.encode_header();
	  client.write(p);
	  read_count = 0;
	}
      } else {
	//clear last prediction
	std::cout << "\033[J";
	word[read_count] = c;
	read_count++;
      }
    }

    client.close();
    t.join();    
    

    
  } catch (std::exception& e)  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
  
  return 0;
  
}

