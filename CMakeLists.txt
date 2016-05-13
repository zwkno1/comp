cmake_minimum_required(VERSION 3.4)

project(comp)

set(CMAKE_CXX_FLAGS "-std=c++14")
add_definitions("-Wall -Werror")

#set(CMAKE_BUILD_TYPE Debug)

include_directories(.)
set(SRCS
	analyzer.cpp
	generator.cpp
	main.cpp
	parser.cpp
	scanner.cpp
	)

add_executable(
	c
	${SRCS}
	)
	
add_executable(
	m
	tm.cpp
	)
