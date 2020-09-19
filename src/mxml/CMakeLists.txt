#set(CMAKE_GENERATOR_PLATFORM x64)
file(GLOB srclist *.cpp *.c *.h config.h.*)
add_library(mxml ${srclist})
