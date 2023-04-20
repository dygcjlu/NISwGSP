cmake_minimum_required(VERSION 3.0)

PROJECT(NISwGSP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#add_definitions("-DNDEBUG")

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})
if(CMAKE_BUILD_TYPE STREQUAL "Debug" )
  #SET(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS "-O0 -g")
  MESSAGE("set debug option type" )
endif()

option(CUDA_ENABLED "Whether to enable CUDA, if available" ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/siftgpu/SiftGPU/cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/siftgpu/SiftGPU/cmake/CMakeHelper.cmake NO_POLICY_SCOPE)

#boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS
             program_options
             filesystem
             graph
             system
             unit_test_framework)
#opengl
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
add_definitions("-DOPENGL_ENABLED")

#glew
find_package(Glew REQUIRED)

#openmp
#add_definitions(-DEIGEN_HAS_OPENMP)
FIND_PACKAGE(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DEIGEN_DONT_PARALLELIZE")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
message("openmp" ${OpenMP_CXX_FLAGS})


#eigen
set(Eigen3_DIR /usr/local/share/eigen3/cmake)
FIND_PACKAGE(Eigen3 REQUIRED)


#glog
find_package(Glog REQUIRED)

#opencv
find_package(OpenCV REQUIRED)	


#cuda

set(CUDA_MIN_VERSION "7.0")
if(CUDA_ENABLED)
    find_package(CUDA ${CUDA_MIN_VERSION} QUIET)
    find_package(CUDAToolkit REQUIRED)
endif()
if(CUDA_FOUND)
    if(CUDA_ENABLED)
        include_directories("${CUDA_INCLUDE_DIRS}")
        add_definitions("-DCUDA_ENABLED")

        include(${CMAKE_CURRENT_SOURCE_DIR}/siftgpu/SiftGPU/cmake/SelectCudaComputeArch.cmake)

        CUDA_SELECT_NVCC_ARCH_FLAGS(CUDA_ARCH_FLAGS ${CUDA_ARCHS})

        set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} ${CUDA_ARCH_FLAGS}")

        # Fix for some combinations of CUDA and GCC (e.g. under Ubuntu 16.04).
        set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -D_FORCE_INLINES")
        # Do not show warnings if the architectures are deprecated.
        set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -Wno-deprecated-gpu-targets")

        message(STATUS "Enabling CUDA support (version: ${CUDA_VERSION_STRING},"
                       " archs: ${CUDA_ARCH_FLAGS_readable})")
    else()
        set(CUDA_FOUND OFF)
        message(STATUS "Disabling CUDA support")
    endif()
else()
    set(CUDA_ENABLED OFF)
    if(CUDA_VERSION_STRING)
        message(STATUS "Disabling CUDA support (found version "
                "${CUDA_VERSION_STRING} but >= ${CUDA_MIN_VERSION} required)")
    else()
        message(STATUS "Disabling CUDA support")
    endif()
endif()

#vlfeat
#FIND_LIBRARY(VLFEAT_LIBRARY NAMES vl HINTS /home/xxd/dyg/image_stitch/mine/NISwGSP/UglyMan_NISwGSP_Stitching/UglyMan_NISwGSP_Stitching/vlfeat-0.9.20/bin/glnxa64)
#FIND_LIBRARY(VLFEAT_LIBRARY NAMES vl HINTS ./vlfeat-0.9.20/bin/glnxa64)
#FIND_LIBRARY(VLFEAT_LIBRARY libvl)

set(COLMAP_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

include_directories( ${COLMAP_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/siftgpu)
#include_directories(${OpenCV_INCLUDE_DIRS})
#include_directories(${EIGEN_INCLUDE_DIRS})

set(COLMAP_LINK_DIRS
    ${Boost_LIBRARY_DIRS}
)

set(COLMAP_EXTERNAL_LIBRARIES
    ${CMAKE_DL_LIBS}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${GLOG_LIBRARIES}
    ${OPENGL_LIBRARIES}
    pthread
)


FIND_LIBRARY(VLFEAT_LIBRARY vlfeat HINTS ./siftgpu/VLFeat/build)
FIND_LIBRARY(SIFTGPU_LIBRARY sift_gpu HINTS ./siftgpu/SiftGPU/build)
FIND_LIBRARY(FLANN_LIBRARY flann HINTS ./siftgpu/FLANN/build)


aux_source_directory(./Debugger DEBUGGER_SRC)
aux_source_directory(./Feature FEATURE_SRC)
aux_source_directory(./Mesh MESH_SRC)
aux_source_directory(./Stitching STITCHING_SRC)
aux_source_directory(./Util UTIL_SRC)
aux_source_directory(./siftgpu/util SIFTGPU_UTIL_SRC)

add_executable(NISwGSP Configure.h main.cpp ./siftgpu/siftgpu_manage.cpp ./siftgpu/siftgpu.cc ./siftgpu/types_.cc ./siftgpu/utils_.cc 
${DEBUGGER_SRC} ${FEATURE_SRC} ${MESH_SRC} ${STITCHING_SRC} ${UTIL_SRC} ${SIFTGPU_UTIL_SRC})
#target_link_libraries(NISwGSP ${OpenCV_LIBS} ${VLFEAT_LIBRARY})
target_link_libraries(NISwGSP ${OpenCV_LIBS} ${VLFEAT_LIBRARY}  ${SIFTGPU_LIBRARY} ${FLANN_LIBRARY} 
${COLMAP_EXTERNAL_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY}  ${OPENGL_glu_LIBRARY} ${CUDA_LIBRARIES})
