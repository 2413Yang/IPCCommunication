#!/bin/bash

# 设置变量
SOURCE_FILE="ArgumentsTableLinux_test.cpp BaseTypeJson.cpp"
OUTPUT_FILE="program"
LIBRARIES="-ljsoncpp"
COMPILER_OPTIONS="-std=c++11 -Wfatal-errors"

# 编译源文件
g++ $COMPILER_OPTIONS $SOURCE_FILE -o $OUTPUT_FILE $LIBRARIES
