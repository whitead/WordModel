#ifndef MODEL_SERVER_H
#define MODEL_SERVER_H

#include <boost/asio.hpp>
#include <deque>
#include <utility>
#include "packet.hpp"
#include "word_model.hpp"


using boost::asio::ip::tcp;
typedef std::deque<Packet> Packet_queue;

template<class M>
class ModelServer : public std::enable_shared_from_this< ModelServer<M> > {
  
public: 
  /**
   * \param wm The word model which will be used. Send using move semantics
   * \param socket An tcp socket open with a client
   * 
   */
  ModelServer(M wm, tcp::socket socket) :
    wm_(std::move(wm)), socket_(std::move(socket)),
    train_packets_(0){
  }

  //This has to be here because shared_from_this can't
  //be invoked from the constructor, which would happen
  //if read_header was called from there
  void start() {
    read_header();
  }

  ~ModelServer() {
    std::cout << "Model server done, bye bye!" << std::endl;
  }
  
private:
  //dispatch packets based on header
  void process_packet() {
    switch(read_packet_.type()){
    case Packet::PACKET_TYPE::PREDICT:
      send_prediction();
      break;
    case Packet::PACKET_TYPE::TRAIN:
      add_to_train();
      break;
    }
  }
    
  //send a prediction given a packet of text
  void send_prediction() {
    //first send the prediction text
    const char* body = read_packet_.body();
    for(int i = 0; i < read_packet_.body_length(); i++)
      wm_.putc(body[i]);
    //retrieve the prediction
    std::string prediction = wm_.get_prediction();
    //construct packet if valid prediction
    if(prediction.size() > 0) {
      Packet p;
      p.body_length(prediction.size() + 1);
      p.type(Packet::PREDICTION);
      std::memcpy(p.body(), prediction.c_str(), p.body_length());
      p.encode_header();

      //write the packet
      bool b_writing = !write_packets_.empty();
      write_packets_.emplace_back(p);
      if(!b_writing) {
	write();
      }    
    }     
  }

  //flush write_packets_
  void write() {
    //This line of code forces a reference
    //to remain to this object and avoid destruction
    auto self(this->shared_from_this());
    boost::asio::async_write(socket_,
      boost::asio::buffer(write_packets_.front().data(),
			write_packets_.front().length()),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
  	  write_packets_.pop_front();
	  if (!write_packets_.empty()) {
	    write();
	  }
	}
      });
  }

  // a write which blocks until complete
  void blocking_write(Packet p) {
    boost::asio::write(socket_,
		       boost::asio::buffer(p.data(),
					   p.length()));
  }


  void read_header() {
    //This line of code forces a reference
    //to remain to this object and avoid destruction
    auto self(this->shared_from_this());
    boost::asio::async_read(socket_,
      boost::asio::buffer(read_packet_.data(), Packet::header_length),
        [this, self](boost::system::error_code ec, std::size_t) {
       if (!ec && read_packet_.decode_header()) {
	 read_body();
       } else {
	 read_header();
       }
      });
  }

  void read_body() {
    //This line of code forces a reference
    //to remain to this object and avoid destruction
    auto self(this->shared_from_this());
    boost::asio::async_read(socket_,
      boost::asio::buffer(read_packet_.body(), read_packet_.body_length()),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
	  process_packet();
	}
	read_header();
      });
  }

//add the text to training data
  void add_to_train() { 
  }

  M wm_;
  tcp::socket socket_;
  Packet read_packet_;
  int train_packets_;
  Packet_queue write_packets_;
  
};

class ModelDispatcher {

public:
  ModelDispatcher(boost::asio::io_service& io_service,
		  const tcp::endpoint& endpoint);

private:
  void process_connect();  
  tcp::acceptor acceptor_;
  tcp::socket socket_;

};

#endif //MODEL_SERVER_H
