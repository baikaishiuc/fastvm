file(GLOB srclist *.cpp *.c *.h)
add_executable(fastvm ${srclist})
target_link_libraries(fastvm mcore Shlwapi)
