#!/usr/bin/env bash

set -e

echo "========================================"
echo " MySchedule 自动化测试开始"
echo "========================================"

echo
echo "[1/9] 清理旧测试数据"
if [ -f "scripts/clean_test_data.sh" ]; then
    bash scripts/clean_test_data.sh
else
    echo "未找到 clean_test_data.sh，跳过清理脚本"
    rm -rf data
    mkdir -p data
fi

echo
echo "[2/9] 查看帮助信息"
./build/myschedule help

echo
echo "[3/9] 注册测试用户"
./build/myschedule register demo 123456 || true

echo
echo "[4/9] 添加普通任务"
./build/myschedule demo 123456 addtask cpp_homework 2026-07-15_20:00 high study 2026-07-15_19:50

echo
echo "[5/9] 查看所有任务"
./build/myschedule demo 123456 showtask all

echo
echo "[6/9] 搜索任务"
./build/myschedule demo 123456 searchtask cpp

echo
echo "[7/9] 修改任务分类"
./build/myschedule demo 123456 edittask 1 category course || true
./build/myschedule demo 123456 showtask all

echo
echo "[8/9] 测试语音 manual 添加任务"
printf "ask voice file task at 2026 0715 2130 priority hi category study remind 2026.07 1521 20\n" | ./build/myschedule demo 123456 voiceadd --manual

echo
echo "[9/9] 删除任务"
./build/myschedule demo 123456 deltask 1 || true
./build/myschedule demo 123456 showtask all

echo
echo "========================================"
echo " MySchedule 自动化测试完成"
echo "========================================"
