add_executable (word_model_test word_model_test)
target_link_libraries (word_model_test
                      word_model_lib
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
		      ${Boost_LIBRARIES})

add_executable (async_test async_test)
target_link_libraries (async_test
                      word_model_lib
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
		      ${Boost_LIBRARIES})

add_custom_target (test ALL
                   ${CMAKE_CURRENT_BINARY_DIR}/word_model_test
                   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/async_test
		   DEPENDS word_model_test
		   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		   COMMENT "Running BOOST tests" VERBATIM)

		 
file (COPY grimm.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file (COPY hello_world_ref.dot DESTINATION ${CMAKE_CURRENT_BINARY_DIR})