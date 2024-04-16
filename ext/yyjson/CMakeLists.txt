# Copyright (C) 2019 Yaoyuan <ibireme@gmail.com>.
# Released under the MIT License:
# https://github.com/ibireme/yyjson/blob/master/LICENSE

cmake_minimum_required(VERSION 3.5)
project(yyjson VERSION 0.9.0 LANGUAGES C)
set(YYJSON_SOVERSION 0)

# ------------------------------------------------------------------------------
# Library
add_library(yyjson src/yyjson.h src/yyjson.c)
target_include_directories(yyjson PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
set_target_properties(yyjson PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${YYJSON_SOVERSION})

