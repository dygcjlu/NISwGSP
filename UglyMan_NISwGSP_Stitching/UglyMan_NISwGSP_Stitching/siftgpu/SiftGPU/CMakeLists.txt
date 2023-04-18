cmake_minimum_required(VERSION 3.0)

project(siftgpu)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CheckCXXCompilerFlag)

# Include helper macros and commands, and allow the included file to override
# the CMake policies in this file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeHelper.cmake NO_POLICY_SCOPE)

option(CUDA_ENABLED "Whether to enable CUDA, if available" ON)

add_definitions("-DSIFTGPU_NO_DEVIL")

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
find_package(Glew REQUIRED)
set(Eigen3_DIR /usr/local/share/eigen3/cmake)
find_package(Eigen3 REQUIRED)


set(COLMAP_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

set(COLMAP_LINK_DIRS
    ${Boost_LIBRARY_DIRS}
)

set(COLMAP_EXTERNAL_LIBRARIES
    ${CMAKE_DL_LIBS}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${OPENGL_LIBRARIES}
    pthread
)


set(SIFT_GPU_SOURCE_FILES
    FrameBufferObject.cpp
    FrameBufferObject.h
    GlobalUtil.cpp
    GlobalUtil.h
    GLTexImage.cpp
    GLTexImage.h
    ProgramGLSL.cpp
    ProgramGLSL.h
    ProgramGPU.h
    PyramidGL.cpp
    PyramidGL.h
    ShaderMan.cpp
    ShaderMan.h
    SiftGPU.cpp
    SiftGPU.h
    SiftMatch.cpp
    SiftMatch.h
    SiftPyramid.cpp
    SiftPyramid.h
)


set(CUDA_MIN_VERSION "7.0")
if(CUDA_ENABLED)
    find_package(CUDA ${CUDA_MIN_VERSION} QUIET)
    
endif()

if(CUDA_ENABLED)
    message("CUDA_ENABLED")
    
    add_definitions("-DCUDA_ENABLED")
    add_definitions("-DCUDA_SIFTGPU_ENABLED")

    set(SIFT_GPU_SOURCE_FILES
        ${SIFT_GPU_SOURCE_FILES}
        CuTexImage.cpp
        CuTexImage.h
        ProgramCU.cu
        ProgramCU.h
        PyramidCU.cpp
        PyramidCU.h
        SiftMatchCU.cpp
        SiftMatchCU.h
    )

    cuda_add_library(sift_gpu STATIC ${SIFT_GPU_SOURCE_FILES})
else()
    cuda_add_library(sift_gpu STATIC ${SIFT_GPU_SOURCE_FILES})
endif()

target_link_libraries(sift_gpu
    ${SIFT_GPU_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
)
