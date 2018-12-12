cmake_minimum_required(VERSION 3.0)

project (zeal)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

include(macros)
include(sanitizers_support)
include(clang_tidy_support)
include(ide_support)

#enable_sanitizers("custom")]

detect_platform()
print_info()
set_output_paths()

add_subdirectory_ex(3rdparty)
set_project_custom_defines()

#set_clang_tidy_args("-checks=-*,clang-analyzer-*,bugprone-*,performance-*,modernize-*,-modernize-use-equals-default")
add_subdirectory_ex(src)