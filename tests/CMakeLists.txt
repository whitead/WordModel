add_executable (word_model_test word_model_test)
target_link_libraries (word_model_test
                      word_model_lib
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
		      ${Boost_LIBRARIES})
add_test ("word model test" word_model_test)
