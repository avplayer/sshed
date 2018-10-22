add_executable(sshed sshed.cpp)
add_executable(sshed.cc sshed.cc)

target_link_libraries(sshed.cc stdc++fs)

install(TARGETS sshed DESTINATION bin)
install(TARGETS sshed.cc DESTINATION bin)