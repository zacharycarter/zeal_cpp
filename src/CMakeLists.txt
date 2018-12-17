file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc *.mm)

add_executable (zeal ${libsrc})
target_include_directories (zeal PUBLIC /usr/local/include/luajit-2.0 ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zeal "-framework Cocoa")