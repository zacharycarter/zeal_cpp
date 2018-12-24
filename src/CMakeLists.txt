file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc *.mm)

add_executable (zeal ${libsrc})
target_include_directories (zeal PUBLIC /Users/zachcarter/projects/luajit-2.0 /Users/zachcarter/projects/bx/include /Users/zachcarter/projects/bgfx/include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zeal "-framework Cocoa" luajit-5.1 bgfx-shared-libDebug)