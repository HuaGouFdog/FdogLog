FdogLog，一个C++的日志库，设置了Error, Warn, Info, Debug, Trace，五种类型日志输出，支持文件写入，自定义输出类型，支持文件滚动，支持多线程，您无需多余操作，只需要向cout一样使用即可，如：

```
#include<iostream>
#include"fdoglogger.h"  //添加日志库头文件

using namespace fdog;   //日志库的命名空间

int main(){

    time_t begin,end;
    double ret;
    begin=clock();
    string name = "花狗Fdog";
    
    FdogInfo("我的名字是" << name);
    return 0;
}
```



#### 一.目录结构

linux文件夹

```
.
├── build
├── CMakeLists.txt
├── conf
│   └── fdoglogconf.conf
├── example
│   ├── CMakeLists.txt
│   └── test.cpp
└── src
    ├── CMakeLists.txt
    └── lib
        ├── CMakeLists.txt
        ├── include
        │   ├── fdoglogger.h
        │   └── filemanagement.h
        └── src
            ├── fdoglogger.cpp
            └── filemanagement.cpp
```

说明：src/lib/src路径下为日志库源文件，src/lib/include路径下为日志库头文件，test文件用于打印日志测试，conf/fdoglogconf.conf文件作为配置文件，关于配置文件的说明，可参考第三条。

windows文件夹

```
.
├── fdoglogger.cpp
├── fdoglogger.h
├── filemanagement.cpp
├── filemanagement.h
└── test.cpp
```





#### 二.编译说明

```
mkdir build  
cd build     
cmake ..
make
```

编译成功后，build目录下会生成bin目录以及lib目录，bin存放的是测试文件，您可以修改随意test.cpp用于测试，lib目录下存放了静态库以及动态库，方便您以后的调用。

注：该方法需要安装cmake以及make，若您不方便安装，可以直接使用g++ 进行编译，这里不再举出。



#### 三.配置文件说明

```
#日志开关 off关闭 on开启，若置为off，则无视logFileSwitch值，logTerminalSwitch值
logSwitch = on
#是否写入文件 off关闭 on开启 决定是否将产生的日志写入文件
logFileSwitch = on
#是否打印在终端 off关闭 on开启 决定是否将产生的日志输出文件 不建议开启
logTerminalSwitch = no
#日志指定等级输出（文件）1：Trace 2：Debug 3：Info 3：Warn 5：Error 表示输出等级，若1,5 表示只打印Trace和Error类型的日志，
logOutputLevelFile = 1,2,3,4,5
#日志指定等级输出（终端）1：Trace 2：Debug 3：Info 3：Warn 5：Error 使用同上
logOutputLevelTerminal =1,2,3,4,5
#日志文件名字 您只需要写名字就好，无需填写后缀
logName = fdoglog
#日志文件保存路径 您可以自定义
logFilePath = /media/rcl/WORK1/FdogLog/logs/
#日志文件最大大小 单位为MB 1024MB=1G
logMixSize = 50
#日志文件达到大小行为执行日志滚动 0为继续写入， 1为立即创建新文件
logBehavior = 1
#日志文件创建行为 单位天数
logOverlay = 1
```



#### 四.日志文件的滚动

如配置文件所示，您可以设置在某个路径生成您的日志文件，同时可以设置一系列行为，比如您设置的是日志创建行为时间为7天，那么首次运行程序，将创建名为fdoglog.log的日志文件，如果中断程序，直到第二次运行，日志库会获取配置中指定的文件最后修改文件的时间，若超过配置天数，旧的fdoglog.log文件会被重命名为fdoglog2021-10-31-22:53:33.log，同时将创建新的fdoglog.log文件进行日志记录，若您开启了日志大小滚动，fdoglog.log达到配置大小也会自动重命名为文件名+时间，并创建新的文件进行写入。日志库暂时没有添加对文件进行删除的功能。



#### 五.测试用例

您只需要加入日志库的头文件fdoglogger.h，并且指定动态库/静态库位置即可，或者更普遍的，您可以直接将源文件放在您的项目之中，正如开头所示代码，你只需要添加一个头文件，以及一个命名空间的语句，即可像cout一样输出您希望输出的东西。

该测试文件位于example路径下的mian.cpp文件，若您无需编译测试文件，可在根目录下CMakeLists.txt中注释掉ADD_SUBDIRECTORY(example)

```
#include<iostream>
#include"fdoglogger.h"  //添加日志库头文件

using namespace fdog;   //日志库的命名空间

int main(){

    time_t begin,end;
    double ret;
    begin=clock();
    string name = "花狗";
    for(int i = 0; i< 10000; i++){
        FdogInfo("我是" << name);
        FdogError("我是" << name);
        FdogWarn("我是" << name);
        FdogInfo("我是" << name);
        FdogDebug("我是" << name);
        FdogTrace("我是" << name);
    }
    end=clock();
    ret=double(end-begin)/CLOCKS_PER_SEC;
    cout<<"runtime:   "<<ret<<endl;
    return 0;
}
```

该测试用例将在单线程环境下输出5W条日志，代码中的ret输出用于打印运行时间，经测试

dev1.1

| 输出类型             | 时间  |
| -------------------- | ----- |
| 只输出到终端         | 1.07s |
| 只输出到文件         | 3.7s  |
| 同时输出到文件和终端 | 5.8s  |

目前的代码逻辑对于文件输出不是很好，有很大的优化空间，下次试试。



dev1.2

| 输出类型             | 时间  |
| -------------------- | ----- |
| 只输出到终端         | 0.95s |
| 只输出到文件         | 1.75s |
| 同时输出到文件和终端 | 2.86s |

针对5W量，dev1.2在同时输出到文件和终端，时间缩进了将近50%。





#### 六.更新

##### 2021年10月31日22:05:34  更新

1. 添加了一个文件管理类，该类专门控制日志文件的创建，以及日志滚动等行为。
2. 输出支持 << 运算符。
3. 添加了CMakeList.txt，可使用Cmake，make进行快速编译，同时会生成静态库以及动态库，方便调用。
4. 不同日志类型支持不同颜色输出。



##### 2021年11月02日18:11:40 更新

1. 文件写入添加是否开始队列策略模式，开启后，日志写入文件的时间将缩短为原有的30%，写入文件和终端总体时间由5.8s缩短了将近50%的2.86%。
2. 删除配置文件中的保存天数logOverlay字段。
3. 支持线程安全。
4. 支持linux，windows双平台。

