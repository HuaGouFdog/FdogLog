#include<iostream>
#include"fdoglogger.h"  //添加日志库头文件

using namespace fdog;   //日志库的命名空间

int main(){
    FdogError("错误");
    FdogWarn("警告");
    FdogInfo("信息");
    FdogDebug("调试");
    FdogTrace("追踪");
    return 0;
}