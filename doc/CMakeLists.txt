# many thanks go to Philippe Poilbarbe for writing the code this file is based on
#  http://www.cmake.org/pipermail/cmake/2006-August/010794.html
#
# much later i also found this:
#  http://tobias.rautenkranz.ch/cmake/doxygen/
# but it is hard to understand...
  
FIND_PACKAGE(Doxygen)

IF(DOXYGEN_FOUND)
  SET( CMAKE_DOXYGEN_IMAGE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/img"
  )

  #Check if we have the graphviz library
  find_program(DOXYGEN_DIAGRAMS dot)

  # doxygen searches for source code (defined in FILE_PATTERNS, for example: *.cpp *.h)
  # with DOXYGEN_SOURCE_DIR we fill a list of directories and later we write it into
  # the Doxyfile with a REGEX REPLACE (see below)
  SET( DOXYGEN_SOURCE_DIR
   "${CMAKE_SOURCE_DIR}/src"
   "${CMAKE_SOURCE_DIR}/include"
   "${CMAKE_SOURCE_DIR}/lib"
  )

  STRING(REGEX REPLACE ";" " " CMAKE_DOXYGEN_INPUT_LIST "${DOXYGEN_SOURCE_DIR}")

  CONFIGURE_FILE(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  ADD_CUSTOM_TARGET(doc 
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM)

#  INSTALL( DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc/" DESTINATION "/usr/share/doc/libnoise-${LIBNOISE_VERSION}" )

ELSE(DOXYGEN_FOUND)
  MESSAGE (FATAL_ERROR "doxygen binary couldn't be found")
ENDIF(DOXYGEN_FOUND)
