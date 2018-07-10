cmake_minimum_required (VERSION 2.6)
project (sshed)
add_executable(sshed sshed.cpp)
install (TARGETS sshed DESTINATION bin)