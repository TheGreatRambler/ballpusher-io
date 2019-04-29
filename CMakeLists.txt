cmake_minimum_required(VERSION 2.8.9)
project(ioGame)
 
# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)
 
# Add all sources
# I have been told it is evil, but oh well
file(GLOB SOURCES "*.cpp")
 
add_executable(ioGame ${SOURCES})