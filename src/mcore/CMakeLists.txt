#set(CMAKE_GENERATOR_PLATFORM x64)
file(GLOB srclist *.cpp *.c *.h)
add_library(mcore ${srclist})