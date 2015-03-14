Word Model Written in C++
======

This is a C++ implementation of a word model

Executables
====

src/echo_server
---
An async io server that echos what's passed to it. 

src/stdin_tcp
---

An async io package that sends stdin to a server as a tcp packet. This
combined with echo server can test the custom packet definition I
have.

src/model_server.cpp
---

This the async io model server that handles creating and sending data
to models. Theoretically can handle multiple models-connections.

NodeJS Files
===

nodejs/local_client.js
---

This creates a local server to host the interaction webpage. 

nodejs/socket_server.js
----

This is the websocket server that communicates with the webpage being served. 

nodejs/model_adaptor.js
---

This is what presents a model for the socket_server to interact
with. Handles packet definition and communication with the async io
model server.

nodejs/packet.js
---
Ths packet definition reimplemented in js. 

Dev Environment
====

1. Start the model-server
2. Start the socket-server, generally as root so you can bind port 80.
3. Start the local-server. 

====

Implementation Notes =====

The C++ program can run as a daemon and communicate via sockets to
send training data and receive predictions. See the [boost asio daemon
example](http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp03/fork/daemon.cpp)
for making a daemon. See the [boost asio udp server
example](http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp)
for receiving training text and sending/requesting predictions. I can
then write a simple NodeJS server to communicatet instead of
implemeting the server in C++.

Bounded context Tree
----

Dealing with new tokens: Should come in at average for each vector. 