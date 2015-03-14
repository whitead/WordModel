#include "model_server.hpp"
#include "simple_model.hpp"
#include "bounded_context_tree_model.hpp"
#include "model_multiplexer.hpp"

#include <fstream>
#include <iostream>

#define BOUNDED

using boost::asio::ip::tcp;

ModelDispatcher::ModelDispatcher(boost::asio::io_service& io_service,
				 const tcp::endpoint& endpoint) :
  acceptor_(io_service, endpoint), socket_(io_service) {
  process_connect();
}

void ModelDispatcher::process_connect() {
  acceptor_.async_accept(socket_,
    [this](boost::system::error_code ec) {
      if(!ec) {
#ifdef BOUNDED
	wordmodel::ModelMultiplexer<wordmodel::BoundedCTModel> mu(1);
	std::cout << "Received connection, creating dispatcher" << std::endl;
	std::make_shared< ModelServer<wordmodel::ModelMultiplexer<wordmodel::BoundedCTModel> > >(std::move(mu), std::move(socket_))->start();	    
#endif // BOUNDED

#ifndef BOUNDED
	wordmodel::ModelMultiplexer<wordmodel::SimpleModel> mu(2);
	std::cout << "Received connection, creating dispatcher" << std::endl;
	std::make_shared< ModelServer<wordmodel::ModelMultiplexer<wordmodel::SimpleModel> > >(std::move(mu), std::move(socket_))->start();
#endif// BOUNDED
      }
      process_connect();
  });
}

int main(int argc, char* argv[]) {

  try  {
    if (argc != 2)
      {
	std::cerr << "Usage: model_server <port or service>" << std::endl;
	return 1;
      }

    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
    ModelDispatcher md(io_service, endpoint);

    io_service.run();


  } catch (std::exception& e)  {
    std::cout << "Exception: " << e.what() << std::endl;
  }

  return 0;
  

}
