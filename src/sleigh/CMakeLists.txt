file(GLOB srclist *.cc *.c *.hh)
add_executable(sleighexample ${srclist})
add_executable(sleigh ${srclist})
add_executable(dobc ${srclist})

target_compile_definitions(sleighexample PUBLIC SLEIGHEXAMPLE)
target_compile_definitions(sleigh PUBLIC SLEIGH)
target_compile_definitions(dobc PUBLIC DOBC)

target_link_libraries(sleighexample mcore Shlwapi)
target_link_libraries(dobc mcore Shlwapi)
