PROJECT(bcu)
cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_BUILD_TYPE Debug)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBFTDI REQUIRED libftdi1)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
project(bcu)
include_directories(${LIBUSB_INCLUDE_DIRS} ${LIBFTDI_INCLUDE_DIRS}include)
include_directories(${PROJECT_SOURCE_DIR})
add_executable(bcu bcu.c port.c chip.c board.c parser.c)
#this link directories is not really needed
link_directories(${LIBUSB_LIBRARY_DIRS} ${LIBFTDI_LIBRARY_DIRS} )
target_link_libraries (bcu ${LIBUSB_LIBRARIES} ${LIBFTDI_LIBRARIES} -lm)
#below is for version number generation
#execute_process(
#  COMMAND git describe --tags --long
#  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#  OUTPUT_VARIABLE GIT_VERSION
#  OUTPUT_STRIP_TRAILING_WHITESPACE
#)
#configure_file(
#  ${CMAKE_SOURCE_DIR}/version.h.in
#  ${CMAKE_BINARY_DIR}/version.h
#)
execute_process( COMMAND ./create_version_h.sh )