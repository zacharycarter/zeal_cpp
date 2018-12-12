add_library (zeal zeal.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../include/zeal/c99/zeal.h)
target_include_directories (zeal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include )

add_library(zeal_shared SHARED zeal.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../include/zeal/c99/zeal.h)
target_link_libraries(zeal_shared)
target_include_directories(zeal_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)