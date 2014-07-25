#include <boost/asio.hpp>
#include <deque>
#include "packet.hpp"

typedef std::deque<Packet> Packet_queue;
using boost::asio::ip::tcp;

class ServiceClient {
public:
  ServiceClient(boost::asio::io_service& io_service,
		tcp::resolver::iterator endpoint_iterator): 
    io_service_(io_service), socket_(io_service) {
    do_connect(endpoint_iterator);
  }

  void write(const Packet& p) {
    io_service_.post([this,p]() {
	bool b_writing = !write_packets_.empty();
	write_packets_.push_back(p);
	if(!b_writing) {
	  do_write();
	}
      });
  }
  
  void close() {
    io_service_.post([this]() { socket_.close(); });
  }

private:
  void do_connect(tcp::resolver::iterator endpoint_iterator)
  {
    boost::asio::async_connect(socket_, endpoint_iterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_packet_.data(), Packet::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_packet_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_packet_.body(), read_packet_.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
	    output_prediction(read_packet_.body());
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void output_prediction(const char* prediction) {
    //change color
    std::cout << "\033[31m";
    //output prediction
    std::cout << prediction;
    //change color back
    std::cout << "\033[0m";
    //move cursor back
    std::cout << "\033[" << std::strlen(prediction) << "D";
    std::cout.flush();
  }

  void do_write()
  {
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_packets_.front().data(),
          write_packets_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_packets_.pop_front();
            if (!write_packets_.empty())
            {
              do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
  }

private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  Packet read_packet_;
  Packet_queue write_packets_;
};
