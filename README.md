Word Model Written in C++
======

This is a C++ implementation of a word model


Implementation Notes
=====

The C++ program can run as a daemon and communicate via sockets to
send training data and receive predictions. See the [boost asio daemon
example](http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/fork/daemon.cpp)
for making a daemon. See the [boost asio udp server
example](http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp)
for receiving training text and sending/requesting predictions. I can
then write a simple NodeJS server to communicatet instead of
implemeting the server in C++.