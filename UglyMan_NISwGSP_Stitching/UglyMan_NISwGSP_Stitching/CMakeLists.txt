cmake_minimum_required(VERSION 3.0)

PROJECT(Test)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#add_definitions("-DNDEBUG")

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})
if(CMAKE_BUILD_TYPE STREQUAL "Debug" )
  #SET(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS "-O0 -g")
  MESSAGE("set debug option type" )
endif()


#opencv
find_package(OpenCV REQUIRED)	

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd)
include_directories( ${OpenCV_INCLUDE_DIRS})

include_directories( "./Util/Networks/udp")

include_directories(./3rd/VideoCapture/include)
link_directories(./3rd/VideoCapture/lib)


aux_source_directory(./Util/Networks/tcp/src TCP_SRC)
aux_source_directory(./Util/Networks/udp UDP_SRC)



add_executable(unit_test unitTest.cpp ./Util/VideoCapture.cpp ./Util/CommonDef.cpp ${TCP_SRC} ${UDP_SRC} )
#target_link_libraries(NISwGSP ${OpenCV_LIBS} ${VLFEAT_LIBRARY})
target_link_libraries(unit_test ${OpenCV_LIBS} -lqcap.x64)
