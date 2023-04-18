cmake_minimum_required(VERSION 3.0)

PROJECT(sift_gpu_demo)

set(CMAKE_BUILD_TYPE Debug)

option(CUDA_ENABLED "Whether to enable CUDA, if available" ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/SiftGPU/cmake)

include(CheckCXXCompilerFlag)

# Include helper macros and commands, and allow the included file to override
# the CMake policies in this file
include(${CMAKE_CURRENT_SOURCE_DIR}/SiftGPU/cmake/CMakeHelper.cmake NO_POLICY_SCOPE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" )
   MESSAGE("set -g flag")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall   -O0 -g -march=native ")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O0 -g -march=native")
   #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
   #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=native")
else()
   MESSAGE("set -O3 flag")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=native")
endif()

#include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#add_definitions(-DEIGEN_HAS_OPENMP)
#FIND_PACKAGE(OpenMP REQUIRED)



set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS
             program_options
             filesystem
             graph
             system
             unit_test_framework)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
add_definitions("-DOPENGL_ENABLED")
message("opengl "${OPENGL_FOUND})

find_package(Glew REQUIRED)

set(Eigen3_DIR /usr/local/share/eigen3/cmake)
find_package(Eigen3 REQUIRED)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DEIGEN_DONT_PARALLELIZE")
find_package(Glog REQUIRED)

#opencv
find_package(OpenCV REQUIRED)	
include_directories(${OpenCV_INCLUDE_DIRS})


#set(CUDA_ARCHS "86" CACHE STRING "List of architectures to generate device code for")
#find_package(CUDA REQUIRED)
# dependent packages
#find_package(CUDAToolkit REQUIRED)

set(CUDA_MIN_VERSION "7.0")
if(CUDA_ENABLED)
    find_package(CUDA ${CUDA_MIN_VERSION} QUIET)
    find_package(CUDAToolkit REQUIRED)
endif()


if(CUDA_FOUND)
    if(CUDA_ENABLED)
        include_directories("${CUDA_INCLUDE_DIRS}")
        add_definitions("-DCUDA_ENABLED")

        include(${CMAKE_CURRENT_SOURCE_DIR}/SiftGPU/cmake/SelectCudaComputeArch.cmake)

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

set(COLMAP_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}

    
)

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

#vlfeat
#FIND_LIBRARY(VLFEAT_LIBRARY NAMES vl HINTS /home/xxd/dyg/image_stitch/mine/NISwGSP/UglyMan_NISwGSP_Stitching/UglyMan_NISwGSP_Stitching/vlfeat-0.9.20/bin/glnxa64)
#FIND_LIBRARY(VLFEAT_LIBRARY NAMES vl HINTS ./vlfeat-0.9.20/bin/glnxa64)

FIND_LIBRARY(VLFEAT_LIBRARY vlfeat HINTS ./VLFeat/build)
FIND_LIBRARY(SIFTGPU_LIBRARY sift_gpu HINTS ./SiftGPU/build)
FIND_LIBRARY(FLANN_LIBRARY flann HINTS ./FLANN/build)

#include_directories(./lib)
include_directories(./)
include_directories( ${COLMAP_INCLUDE_DIRS})
#include_directories(/home/xxd/dyg/image_stitch/mine/NISwGSP/UglyMan_NISwGSP_Stitching/UglyMan_NISwGSP_Stitching/siftgpu)CUDA::cudart
#include_directories(./VLFeat)
#include_directories(./util)
#include_directories(./FLANN)

aux_source_directory(./util UTIL_SRC)

add_executable(sift_gpu_demo  siftgpu_demo.cpp siftgpu.cc siftgpu.h types_.cc types_.h utils_.cc utils_.h ${UTIL_SRC})
target_link_libraries(sift_gpu_demo ${OpenCV_LIBS} ${VLFEAT_LIBRARY}  ${SIFTGPU_LIBRARY} ${FLANN_LIBRARY} 
${COLMAP_EXTERNAL_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY}  ${OPENGL_glu_LIBRARY} ${CUDA_LIBRARIES})
