cmake_minimum_required(VERSION 3.10)

project(cpp-socks CXX)

file(GLOB SOURCES
	"src/*.cpp"
	"src/*.h"
	)
add_executable(${PROJECT_NAME} ${SOURCES})

