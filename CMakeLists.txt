project(bg-generation-penrose)

#**************************************************************************************************
# General cMake settings **************************************************************************
cmake_minimum_required(VERSION 3.17)

#**************************************************************************************************
# Find Package ************************************************************************************
find_package(cxxopts CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)


#**************************************************************************************************
# Include *****************************************************************************************
include_directories(${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

#**************************************************************************************************
# Set compiler ************************************************************************************

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_compile_options(/std:c++latest /Oy /utf-8)
else()
  add_compile_options(-std=c++2a -fPIC -O3 -fno-omit-frame-pointer)
endif()

#**************************************************************************************************
# Set variable ************************************************************************************
SET(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

#**************************************************************************************************
# Make configuration ******************************************************************************
add_executable(bg-generation-penrose ${SOURCES})
target_link_libraries(bg-generation-penrose fmt::fmt spdlog::spdlog)
