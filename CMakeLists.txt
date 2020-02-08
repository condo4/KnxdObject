cmake_minimum_required(VERSION 2.8)
project(KnxdObject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES knxdconnection.cpp knxdconnection.h utils.h dpt.h)

add_library(KnxdObject STATIC ${SOURCE_FILES})
