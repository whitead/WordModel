#include <boost/asio.hpp>
#include <deque>
#include "packet.hpp"

typedef std::deque<Packet> Packet_queue;
using boost::asio::ip::tcp;

class ServiceServer {
public:
  ServiceServer(boost::asio::io_service& io_service,
		const tcp::endpoint& endpoint): 
    socket_(io_service),  acceptor_(io_service, endpoint){
    do_accept();
  }

  void deliver_packet(Packet p) {
    bool b_writing = !write_packets_.empty();
    //push back should do a copy, so we don't have to worry about the 
    //using the read_packet_ here
    write_packets_.push_back(p);
    if(!b_writing) {
      do_write();
    }
  }

private:

  void do_accept() {
    acceptor_.async_accept(socket_,
			  [this](boost::system::error_code ec) {

			    if(!ec) {
			      std::cout << "About to read header" <<std::endl;
			      do_read_header();
			    }

			    do_accept();
			  });
  }

 void do_read_header() {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_packet_.data(), Packet::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_packet_.decode_header())
          {
            do_read_body();
          }
        });
  }

  void do_read_body() { 
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_packet_.body(), read_packet_.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
	    std::cout << "---------------------" << std::endl;
	    std::cout << read_packet_.type_string() << std::endl;
	    std::cout << read_packet_.body() << std::endl;
	    std::cout << "---------------------" << std::endl;
	    //now send packet back. 	    
	    deliver_packet(read_packet_);
	    //continue reading
	    do_read_header();
          }
        });
  }
  
  void do_write() {
    boost::asio::async_write(socket_,
	boost::asio::buffer(write_packets_.front().data(),
			    write_packets_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
			       if (!ec) {
				 write_packets_.pop_front();
				 if (!write_packets_.empty())
				   {
				     do_write();
				   }
			       }
			     });
  }
  
  Packet read_packet_;
  Packet_queue write_packets_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

int main(int argc, char* argv[]) {

  try  {
    if (argc != 2)
      {
	std::cerr << "Usage: echo_server <port or service>" << std::endl;
	return 1;
      }

    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
    ServiceServer s(io_service, endpoint);

    io_service.run();


  } catch (std::exception& e)  {
    std::cout << "Exception: " << e.what() << std::endl;
  }

  return 0;
  

}
