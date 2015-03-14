cmake_minimum_required (VERSION 2.6)

#project info
project (WordModel)
set (WordModel_VERSION_MAJOR 0)
set (WordModel_VERSION_MINOR 1)
set (WordModel_DESCRIPTION "A C++ syntax and topic word model")


#compilation flags
set (CMAKE_CXX_FLAGS "-std=c++11 -Ofast -march=native -g")
#set (CMAKE_CXX_FLAGS "-std=c++11 -O0 -DDEBUG_PARSER -g")

add_definitions (${CMAKE_CXX_FLAGS})


#set-up directories
include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_SOURCE_DIR}/src")
add_definitions (-DCORPUS_DIR="${PROJECT_SOURCE_DIR}/tests")


#Detect boost
find_package (Boost 1.36 COMPONENTS thread system filesystem unit_test_framework REQUIRED)  
include_directories (${Boost_INCLUDE_DIRS})


#compile library
add_subdirectory ("lib")
link_directories("${PROJECT_SOURCE_DIR}/lib")

#compile src
add_subdirectory ("src")

#compile unit tests
add_subdirectory ("tests")

#generate doc
add_subdirectory("doc")