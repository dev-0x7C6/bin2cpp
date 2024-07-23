file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE headers *.hpp)

add_executable(bin2cpp ${sources} ${headers})
target_link_libraries(bin2cpp fmt::fmt)
install(TARGETS bin2cpp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
