---

# MySchedule: GUI 与语音任务解析分支说明

## 概述

本分支为 **MySchedule** 项目引入了两个主要的核心增强功能：

1. 为 Linux 环境构建的基于 Qt 的全交互式图形用户界面 (GUI)。
2. `VoiceTaskParser` (语音任务解析) 模块，用于从语音识别提取的纯文本中解析并构建结构化的日程数据。

这些功能极大地完善了项目的交互体验，提供了直观的可视化操作面板，打通了自然语言与底层任务管理系统之间的桥梁。

## 目录结构

当前模块的文件结构如下：

```text
├── gui/
│   ├── main_gui.cpp        # GUI 应用程序的主入口
│   ├── MainWindow.h        # Qt 主窗口头文件，定义了 UI 组件与成员变量
│   └── MainWindow.cpp      # 核心 UI 实现，包含布局排版、信号槽连接及表单输入校验
├── include/
│   ├── Utils.h             # 公共工具类接口（时间处理、哈希加密等）
│   └── VoiceTaskParser.h   # 语音解析模块接口与 VoiceTaskInfo 数据结构
└── src/
    ├── main.cpp            # 命令行(CLI)入口，集成了语音脚本调用与系统整合
    ├── Utils.cpp           # 日期时间格式校验与字符串工具的底层实现
    └── VoiceTaskParser.cpp # 核心解析逻辑：正则匹配、关键字提取与属性归一化

```

## 核心功能详情

### 1. 交互式图形界面 (Qt GUI 模块)

GUI 模块为 `TaskManager` 提供了完整的可视化前端操作面板。

* **安全验证拦截：** 包含完整的登录态拦截，密码在底层经过 Hash 处理后验证，验证通过后方可激活操作面板。
* **任务管理 (CRUD)：** 提供完善的表单和数据表格，支持任务的添加、修改选中项、删除以及标记完成。
* **实时检索与过滤：** 实现了强大的多维过滤器，支持通过关键字（名称/类别/优先级/状态）、任务状态（pending/done）和优先级进行实时交叉筛选。
* **健壮的输入校验：** 拦截异常输入（如名称或时间为空），并在提交给 `TaskManager` 前通过 `QMessageBox` 给出明确的警告提示，避免程序崩溃或产生脏数据。
* **后台自动提醒：** 内置 `QTimer` 定时器（每 30 秒轮询一次），在 GUI 运行期间持续监听任务状态并触发到期提醒。

### 2. 语音任务解析引擎 (Voice Task Parser)

该模块负责将 Python 脚本识别出的非结构化自然语言文本，精准转化为标准化的 `VoiceTaskInfo` 对象。

* **关键字锚定 (Keyword Anchoring)：** 智能识别 `at`、`priority`、`category` 和 `remind` 等语法关键字，精准拆分任务语句。
* **属性归一化 (Fuzzy Normalization)：** 自动将口语化的表达（如 "urgent", "mid"）映射并规范为系统标准的优先级 ("high", "medium", "low") 或类别。
* **日期时间提取与重组：** 能够从连串的数字中提取 11 位或 12 位的时间信息，并自动重组为系统强制要求的 `YYYY-MM-DD_HH:MM` 格式。
* **安全名称过滤：** 自动清理任务名称中的无效字符，将空格及不合法字符替换为下划线，确保数据入库的安全性。

## API 与使用示例

### 启动 GUI 模式

在完成 Qt 项目编译后，直接运行 GUI 二进制文件。入口函数将自动接管并启动 Qt 事件循环：

```cpp
// 来源: gui/main_gui.cpp
#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

```

### 调用语音解析接口

在处理语音命令时，仅需调用静态方法 `VoiceTaskParser::parse`，传入识别到的文本字符串及数据结构引用即可：

```cpp
#include "VoiceTaskParser.h"
#include <iostream>

int main() {
    // 模拟语音脚本返回的识别文本
    std::string recognizedText = "task WriteReport at 2026-07-15 20:30 priority urgent category study remind 2026-07-15 20:20";
    VoiceTaskInfo task;

    if (VoiceTaskParser::parse(recognizedText, task)) {
        std::cout << "任务名称: " << task.name << "\n";       // 输出: WriteReport
        std::cout << "开始时间: " << task.startTime << "\n";  // 输出: 2026-07-15_20:30
        std::cout << "优先级: " << task.priority << "\n";    // 输出: high (已归一化)
        std::cout << "任务分类: " << task.category << "\n";    // 输出: study
    } else {
        std::cerr << "语音命令解析失败，请检查格式。\n";
    }
    return 0;
}

```
