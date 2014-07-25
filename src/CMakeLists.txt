include_directories(${Boost_INCLUDE_DIRS})
add_executable (stdin_tcp stdin_tcp)
add_executable (echo_server echo_server)
add_executable (model_server model_server)
target_link_libraries (stdin_tcp
                       word_model_lib
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
		       ${Boost_LIBRARIES})
target_link_libraries (echo_server
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
		       ${Boost_LIBRARIES})
target_link_libraries (model_server
                       word_model_lib
                       ${Boost_THREAD_LIBRARY}
                       ${Boost_SYSTEM_LIBRARY}
		       ${Boost_LIBRARIES})
