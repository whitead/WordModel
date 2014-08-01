cmake_minimum_required (VERSION 2.6)

#project info
project (WordModel)
set (WordModel_VERSION_MAJOR 0)
set (WordModel_VERSION_MINOR 1)
set (WordModel_DESCRIPTION "A C++ syntax and topic word model")

#compilation flags
#set (CXX_STD_VERSION "-std=c++11 -g")
set (CXX_STD_VERSION "-std=c++11 -O2 -ftree-vectorize -g")
add_definitions (${CXX_STD_VERSION})


#set-up directories
include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_SOURCE_DIR}/src")


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