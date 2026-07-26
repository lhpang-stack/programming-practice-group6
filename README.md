# MySchedule 个人日程管理系统 - 任务管理与提醒模块

开发者：田卓凝
当前分支：feature/task-manager-reminder

## 1. 分支说明
本分支主要负责 MySchedule 项目的任务操作逻辑和提醒功能，是整个系统的核心数据处理部分。

## 2. 实现功能
- 任务管理 (TaskManager)：
  实现了任务的添加、删除、修改、查询，以及标记任务完成状态。
  负责任务数据的本地持久化，将任务列表保存到 txt 文件中，并在程序启动时自动加载。
  处理了非法任务编号等边界情况，防止程序崩溃。

- 提醒逻辑 (Reminder)：
  实现了任务时间的校验逻辑，遍历任务列表判断是否到达提醒时间。
  自动跳过已完成的任务，避免无效提醒。

- 声音提醒 (SoundReminder)：
  定义了声音提醒类，实现了播放提醒的底层接口。

## 3. 负责文件
- include/TaskManager.h
- src/TaskManager.cpp
- include/Reminder.h
- src/Reminder.cpp
- include/SoundReminder.h
- src/SoundReminder.cpp
