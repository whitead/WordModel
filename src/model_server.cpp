#include "model_server.hpp"
#include "simple_model.hpp"
#include "bounded_context_tree_model.hpp"

#include <fstream>
#include <iostream>

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
	wordmodel::BoundedCTModel sm;
	std::cout << "Received connection, creating dispatcher" << std::endl;
	std::make_shared< ModelServer<wordmodel::BoundedCTModel> >(std::move(sm), std::move(socket_))->start();
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
