# 模型、用户与工具模块说明文档

## 1. 模块概述

本模块对应分支：feature/model-user-utils。

该模块主要负责项目中的基础数据模型、用户管理逻辑和通用工具校验功能，为任务管理、提醒、语音输入和图形界面等模块提供基础支持。

本模块已完成以下内容：

- Task 任务数据模型
- UserManager 用户注册与登录管理
- Utils 通用输入校验工具
- UserManager 与 Utils 的基础整合

---

## 2. 文件结构

本模块新增和维护的主要文件如下：

| 路径 | 说明 |
|---|---|
| include/Task.h | Task 任务模型头文件 |
| src/Task.cpp | Task 任务模型实现文件 |
| include/UserManager.h | UserManager 用户管理头文件 |
| src/UserManager.cpp | UserManager 用户管理实现文件 |
| include/Utils.h | Utils 工具函数头文件 |
| src/Utils.cpp | Utils 工具函数实现文件 |

---

## 3. Task 任务模型

Task 类用于表示系统中的一个任务对象。

主要职责包括：

- 保存任务编号
- 保存任务标题
- 保存任务描述
- 保存任务日期
- 保存任务时间
- 保存任务完成状态
- 提供任务属性的读取与修改接口

该模型可以被任务管理模块、提醒模块和界面模块复用。

---

## 4. UserManager 用户管理模块

UserManager 类用于处理用户相关逻辑。

目前支持的功能包括：

- 用户注册
- 用户名重复检查
- 用户登录
- 用户登出
- 判断当前是否已有用户登录
- 获取当前登录用户名
- 从本地文件加载用户数据
- 将用户数据保存到本地文件

用户数据默认存储位置为：

| 项目 | 内容 |
|---|---|
| 默认路径 | data/users.txt |
| 存储格式 | username,password |
| 示例内容 | alice,123456 |

说明：当前项目是课程练习项目，密码暂时以明文方式保存。真实项目中不应这样处理，实际开发中应使用哈希加密后再存储。

---

## 5. Utils 工具校验模块

Utils 模块提供通用的输入处理和格式校验函数，便于多个模块复用。

目前包含的功能如下：

| 函数 | 作用 |
|---|---|
| trim | 去除字符串首尾空白字符 |
| isValidUsername | 校验用户名是否合法 |
| isValidPassword | 校验密码是否合法 |
| isValidDate | 校验日期格式是否合法 |
| isValidTime | 校验时间格式是否合法 |
| isPositiveInteger | 校验字符串是否为正整数 |

当前校验规则如下：

| 校验项 | 规则 |
|---|---|
| 用户名 | 长度为 3 到 20 位，只允许字母、数字和下划线 |
| 密码 | 长度为 6 到 30 位，且不能全为空白字符 |
| 日期 | 格式为 YYYY-MM-DD，并检查月份和日期范围 |
| 时间 | 格式为 HH:MM，并检查小时和分钟范围 |
| 正整数 | 字符串只能包含数字，且数值必须大于 0 |

---

## 6. 模块整合说明

UserManager 已经接入 Utils 中的部分校验函数。

具体整合点包括：

- 注册用户时，使用 trim 处理用户名首尾空白
- 注册用户时，使用 isValidUsername 校验用户名
- 注册用户时，使用 isValidPassword 校验密码
- 登录用户时，使用 trim 处理用户名首尾空白
- 登录用户时，使用 isValidUsername 初步校验用户名格式
- 加载用户数据时，会忽略不符合用户名格式的记录

这样可以减少重复判断逻辑，并提高不同模块之间的输入校验一致性。

---

## 7. 与其他模块的对接方式

其他模块可以根据需要使用本模块提供的头文件。

| 使用场景 | 建议使用模块 |
|---|---|
| 创建和展示任务 | Task |
| 管理任务集合 | Task 配合 TaskManager |
| 用户注册和登录 | UserManager |
| 图形界面中的输入校验 | Utils |
| 语音解析后的日期和时间校验 | Utils |
| 提醒模块中的任务时间判断 | Task 和 Utils |

后续对接时建议注意：

- 任务管理模块优先复用 Task 类，不要重复定义任务结构。
- 界面模块如需登录功能，应调用 UserManager，而不是重新实现用户系统。
- 语音输入模块如需校验日期、时间或数字，应优先复用 Utils。
- 如果不同成员都定义了 Utils，需要在合并时统一函数命名和实现逻辑。

---

## 8. 当前开发状态

当前分支：feature/model-user-utils。

已完成提交内容包括：

| 序号 | 提交信息 |
|---|---|
| 1 | feat: add task data model header |
| 2 | feat: implement task data model |
| 3 | feat: add user manager interface |
| 4 | feat: implement user registration logic |
| 5 | feat: implement user login logic |
| 6 | feat: add utility validation interface |
| 7 | feat: implement utility validation helpers |
| 8 | refactor: improve model and user utility integration |

当前模块状态：

- 功能代码已完成
- 已推送到远程分支
- 工作区保持干净
- 可进入代码审查和分支合并阶段

---

## 9. 后续合并注意事项

合并到 dev 分支时，需要重点关注以下可能冲突的文件：

| 文件 | 冲突风险 | 原因 |
|---|---|---|
| include/Task.h | 中 | 任务管理分支可能也修改了 Task 定义 |
| src/Task.cpp | 中 | 任务管理分支可能也实现了 Task 方法 |
| include/UserManager.h | 中 | 任务管理分支也出现了 UserManager 文件 |
| src/UserManager.cpp | 中 | 任务管理分支可能也实现了用户管理逻辑 |
| include/Utils.h | 高 | 语音界面分支也有 Utils.h |
| src/Utils.cpp | 高 | 语音界面分支也有 Utils.cpp |

建议合并顺序：

1. 先将 feature/model-user-utils 合并到 dev。
2. 再合并 feature/task-manager-reminder。
3. 最后合并 feature/interface-voice-gui。
4. 合并 Utils 时保留通用能力，并兼容语音解析所需函数。
5. 合并 UserManager 时保留注册、登录、登出、当前用户状态和文件读写能力。

---

## 10. 总结

本模块为项目提供了基础模型、用户系统和输入校验能力，是其他功能模块的基础依赖。

当前 feature/model-user-utils 分支已经完成开发，可以作为后续任务管理、提醒系统、语音输入和图形界面的基础模块使用。
