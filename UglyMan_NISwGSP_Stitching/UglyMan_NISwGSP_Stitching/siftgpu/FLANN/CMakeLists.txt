cmake_minimum_required(VERSION 3.0)

project(flann)

#COLMAP_ADD_STATIC_LIBRARY(flann
#    flann.hpp flann.cpp
#    ext/lz4.h ext/lz4.c
#    ext/lz4hc.h ext/lz4hc.c
#)

#include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/..)

add_library(flann STATIC 
flann.hpp 
flann.cpp
ext/lz4.h 
ext/lz4.c
ext/lz4hc.h
ext/lz4hc.c )

#set_target_properties(flann PROPERTIES FOLDER ${COLMAP_TARGETS_ROOT_FOLDER}/${FOLDER_NAME})
