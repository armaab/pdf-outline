cmake_minimum_required(VERSION 2.6)
project(pdf-outline)
find_package(PkgConfig)
pkg_check_modules (poppler poppler  REQUIRED )
add_executable(${PROJECT_NAME} main.cpp)
include_directories( ${poppler_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${poppler_LIBRARIES} )
