#!/bin/bash

./run 

exit_status = $?
echo $exit_status
# 检查退出状态码
if [ $exit_status -eq 0 ]; then
    echo "C++程序成功执行"
else
    echo "C++程序执行失败，返回值为 $exit_status"
fi