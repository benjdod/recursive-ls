cmake_minimum_required(VERSION 3.10)

project(els VERSION 0.1.0)

add_executable(els
	src/main.c
	src/chartrie.c
	src/chartrie.h
	src/filenamebuffer.c
	src/filenamebuffer.h
)
