#include<iostream>
#include"fdoglogger.h"  //添加日志库头文件

using namespace fdog;   //日志库的命名空间

int main(){

    time_t begin,end;
    double ret;
    begin=clock();
    string name = "花狗";
    for(int i = 0; i< 10000; i++){
        FdogError("我是" << name << i);
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