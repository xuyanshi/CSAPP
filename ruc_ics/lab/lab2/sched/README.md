# 调度器题目评分程序

## 目录结构

根目录下`sim_xxx.sh`文件是具体语言实现的策略的评分脚本，目前它们将运行所有测试点，并输出结果。其中以`_rr`结尾的脚本使用的是round robin调度策略，用于测试目的。`trace_gen.sh`是测试点生成脚本，生成的trace保存在`traces`文件夹下，不需要每次运行次脚本。评分程序（模拟器）用C++实现，代码位于`cpp`文件夹；python和java策略接口以long-running独立程序形式与模拟器通信（使用pipe），它们的源代码分别位于`python`和`java`文件夹。`config`文件夹包括trace生成脚本的配置信息与模拟器的运行配置信息，正常情况不需要调整。

## 选手提交的文件

### Python接口

选手修改`python/policy.py`文件。

### Java接口

选手修改`java/src/Policy.java`文件。

### Java接口

选手修改`cpp/src/policy.cc`文件。

## 评分

针对选手选择的不同接口，运行`sim_cpp.sh`、`sim_java.sh`或`sim_py.sh`来启动评分程序。目前脚本一次运行将编译并启动评分程序，评测所有子任务，并输出评分结果。评分程序运行方式需要根据OJ需求进一步调整。

### 子任务

`traces`文件夹里每个文件对应题面中提及的子任务，是评分程序的输入。

## 配置程序

### C++ 部分运行说明

因为评分程序是由C++编写，C++的策略实现可以与评分程序编译为一个可执行文件。（相对的，Java和Python策略实现是通过进程间通信与评分程序互动的）。

### Java 部分运行说明

测试机器java构建环境：
maven版本：“Apache Maven 3.6.2”
jdk版本：“Java version: 12.0.2”
操作系统：OS name: "mac os x", version: "10.14.6"

程序说明：
* Main：选择要采用的调度器，监听标准输入，解析成事件Event类以及其他模拟器实时信息，返回调度器调度结果（通过标准输出）
* BaseScheduler：调度器类型的抽象父类，定义了调度器子类需要实现的调度接口`schedule()`，并提供了（反）序列化json格式的实现
* RoundRobinScheduler：默认实现的Round-Robin算法，可在Main中指定使用
* FIFOScheduler：默认实现的FIFO算法，可在Main中指定使用
* Scheduler：需要选手实现的调度器，未提供`schedule()`实现
* Event等：模型对象，通过json字符串解析得到，作为`schedule()`输入

### Python 部分运行说明

测试机器python构建环境：
python版本：“Python 2.7.10”

程序说明：
* scheduler.py：选择要采用的调度器，监听标准输入，解析成python dictionary类型，并同其他模拟器实时信息作为函数输入，调用调度接口`policy()`，返回调度器调度结果（通过标准输出）
* policy_rr.py：默认实现的Round-Robin算法，可在`scheduler.py`中指定使用
* policy_filo.py：默认实现的FIFO算法，可在`scheduler.py`中指定使用
* policy.py：需要选手实现的调度器，未提供`policy()`实现

### main.c 文件说明

参数1：运行的trace文件名，默认文件在`/traces`文件夹下
参数2：模拟器为java还是python，根据此启动不同的调度器程序

使用`pipe`重定向模拟器模块与调度器模块的标准输入以及标准输出，实现模拟器发送事件，调度器响应处理返回调度结果的信息传递过程。
启动程序，开始任务。