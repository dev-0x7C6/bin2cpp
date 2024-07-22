file(GLOB_RECURSE sources *.cpp)

add_executable(bin2cpp ${sources})
target_link_libraries(bin2cpp fmt::fmt)
install(TARGETS bin2cpp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
