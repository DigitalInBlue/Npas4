#
# Top level makefile for npas4
#
# Copyright 2014 John Farrier 
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#

#
# Cmake Configuration
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Project Name
PROJECT(npas4Project)

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)

#
# User Options
#

option(NPAS4_COMPILE_DYNAMIC_LIBRARIES "Set to ON to build npas4 for dynamic linking.  Use OFF for static." ON)
option(NPAS4_RUN_EXAMPLE_ON_BUILD "Set to ON to automatically run the example after a successful build." ON)
option(NPAS4_USE_FOLDERS "Enable to put npas4 in its own solution folder under Visual Studio" ON)
option(NPAS4_ENABLE_TESTS "Enable building and running unit tests." ON)

if(NPAS4_COMPILE_DYNAMIC_LIBRARIES)
	SET(NPAS4_USER_DEFINED_SHARED_OR_STATIC "SHARED")
else()
	SET(NPAS4_USER_DEFINED_SHARED_OR_STATIC "STATIC")
endif()

if(NPAS4_ENABLE_TESTS)
	set(GTEST_DIRECTORY test/gtest-1.7.0)
	set(GTEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/include)
	set(GTEST_LIBRARY gtest)
	set(GTEST_MAIN_LIBRARY gtest_main)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/include ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/src)
	ADD_SUBDIRECTORY(${GTEST_DIRECTORY})
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# Compiler Settings
#

if(MSVC) 
  # Force to always compile with warning level 4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]") 
	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") 
  else() 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4") 
  endif() 

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Compiler Flags for Debug Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE) 

  add_definitions("/D_CRT_SECURE_NO_WARNINGS /wd4251 /nologo")
endif() 
 
if(BORLAND) 
  set(CMAKE_CXX_FLAGS "" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "Compiler Flags for Debug Builds" FORCE)  
  set(CMAKE_CXX_FLAGS_RELEASE  "" CACHE STRING "Compiler Flags for Release Builds" FORCE) 
endif() 

message (STATUS "SYSTEM: ${CMAKE_SYSTEM_NAME}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# EXEC_PROGRAM(xcrun ARGS -find clang++ OUTPUT_VARIABLE XCODE_CLANG_PATH)
	set (XCODE_CLANG_PATH "${XCODE_CLANG_PATH}" CACHE STRING "Xcode clang path")
	set (CMAKE_CXX_COMPILER "${XCODE_CLANG_PATH}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") 
	set (CMAKE_COMPILER_IS_GNUCXX 1)
endif()
 
if(UNIX)
	if(CMAKE_COMPILER_IS_GNUCXX) 
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11 -Wall" CACHE STRING "Compiler Flags for All Builds" FORCE) 
	  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic" CACHE STRING "Compiler Flags for Debug Builds" FORCE) 
	  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE) 
	else()
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11 -Wall -fPIC" CACHE STRING "Compiler Flags for All Builds" FORCE) 
	  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic" CACHE STRING "Compiler Flags for Debug Builds" FORCE) 
	  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE) 
	endif() 
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Build and Install Settings
#

set(CMAKE_DEBUG_POSTFIX          "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX        ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX     ""  CACHE STRING "add a postfix, usually empty on windows")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# --------------------------------------------------------------------------- 
# --------------------------------------------------------------------------- 

#
# Install Locations
#

#
# include path to be used by all projects
#

SET(HEADER_PATH ${npas4_SOURCE_DIR}/include)

set(PROJECT_NAME npas4)

if(NPAS4_COMPILE_DYNAMIC_LIBRARIES)
	add_definitions(-DNPAS4_EXPORTS)
else()
	add_definitions(-DNPAS4_STATIC)
endif()

#
# Define header and sources
#

set(TARGET_H
	include/npas4/Npas4.h
)

set(TARGET_SRC
	src/Npas4.cpp
)

set(TARGET_LIBRARIES ${SYSLIBS})
add_library(${PROJECT_NAME} ${NPAS4_USER_DEFINED_SHARED_OR_STATIC} ${TARGET_SRC} ${TARGET_H})
include_directories(${HEADER_PATH})

# --------------------------------------------------------------------------- 
# Google Test Application
# --------------------------------------------------------------------------- 

if(NPAS4_ENABLE_TESTS)
	set(PROJECT_NAME npas4Test)

	add_executable(${PROJECT_NAME} 
		test/npas4/Npas4.test.cpp
		)

	# VS2012 doesn't support true variadic templates
	if(MSVC) 
		add_definitions( /D _VARIADIC_MAX=10 )
	endif()

	SET(HEADER_PATH ${npas4_SOURCE_DIR}/include)
	include_directories(${HEADER_PATH})
	include_directories(${GTEST_INCLUDE_DIR})

	add_dependencies(${PROJECT_NAME} npas4)
	target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} npas4)

	if(NPAS4_ENABLE_AUTO_RUN_TESTS)
		add_test(${PROJECT_NAME} ${PROJECT_NAME})
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND $<TARGET_FILE:${PROJECT_NAME}>)
	endif()

	if(NPAS4_USE_FOLDERS)
		set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "npas4/Test")
	endif()
endif()

# --------------------------------------------------------------------------- 
# Optional
# --------------------------------------------------------------------------- 

if(NPAS4_USE_FOLDERS)
	set_property(TARGET npas4 PROPERTY FOLDER "npas4")
endif()
