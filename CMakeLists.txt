cmake_minimum_required(VERSION 3.0)  # Minimum required CMake version

project(politics)  # Name of your project

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_compile_options(-std=c++20 -Wall -pedantic)

# Add the source files to your project
set(SOURCES
    src/ParseHTML.cpp
    src/Politician.cpp
    src/House.cpp
    src/House_of_Representatives.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(LIBS gumbo curl)
set(CMAKE_BUILD_RPATH "/usr/local/lib")

ADD_CUSTOM_TARGET(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the debug mode.")

ADD_CUSTOM_TARGET(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the release mode.")

# Set the executable name and link the source files
add_executable(US_House_Parsed_HTML ${SOURCES} ${HEADERS})
target_link_libraries(US_House_Parsed_HTML ${LIBS})
target_compile_definitions(US_House_Parsed_HTML PUBLIC PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

# add_executable(bankingTest test/bankingTest.c)
# target_link_libraries(bankingTest cunit)