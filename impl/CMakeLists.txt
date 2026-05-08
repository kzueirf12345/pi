cmake_minimum_required(VERSION 3.21)
project(pi LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

###===========================================FLAGS=================================================

option(SANITIZE "Enable Sanitizers" OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_library(project_warnings INTERFACE)
    target_compile_options(project_warnings INTERFACE
        -Wall -Wextra -Werror -Wpedantic -march=native
        $<$<CONFIG:DEBUG>:-Og -g>
        $<$<CONFIG:RELEASE>:-O3 -DNDEBUG>
    )
endif()

if(SANITIZE AND NOT MSVC)
    add_library(project_sanitizers INTERFACE)
    target_compile_options(project_sanitizers INTERFACE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
    )
    target_link_options(project_sanitizers INTERFACE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
endif()

###===========================================PACKAGES==============================================

find_path(MPFR_INCLUDE_DIR mpfr.h)
find_path(GMP_INCLUDE_DIR gmp.h)

find_library(MPFR_LIBRARY mpfr)
find_library(GMP_LIBRARY gmp)

###===========================================SUBDIRECTORY==========================================

add_subdirectory(source/libs)
add_subdirectory(source/Testing)
add_subdirectory(source/CliParser)
add_subdirectory(source/Measurer)
add_subdirectory(source/Calculation)

add_executable(pi source/main.cpp)

###===========================================LIB_LINK==============================================

target_link_libraries(pi PRIVATE project_warnings)

if(SANITIZE AND NOT MSVC)
    target_link_libraries(pi PRIVATE project_sanitizers)
endif()

target_link_libraries(pi PRIVATE
    gmp
    mpfr

    libs
    Testing
    CliParser
    Measurer
    Calculation
)

target_include_directories(pi PRIVATE 
    mpfr
    gmp
)