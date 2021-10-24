#ifndef FDOGLOGGER_H
#define FDOGLOGGER_H
#include<iostream>
#include<fstream>
#include<map>
#include<mutex>
#ifndef linux
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <pwd.h>
#endif
#ifndef WIN32
//TODO
#endif
using namespace std;

namespace fdog {

#define RED   "\e[1;31m"
#define BLUE  "\e[1;34m"
#define GREEN "\e[1;32m"
#define WHITE "\e[1;37m"
#define DEFA  "\e[0m"

enum class coutType: int {Error, Warn, Info, Debug, Trace};
enum class fileType: int {Error, Warn, Info, Debug, Trace};
enum class terminalType: int {Error, Warn, Info, Debug, Trace};

struct Logger {
    string logSwitch;           //日志开关
    string logFileSwitch;       //是否写入文件
    string logTerminalSwitch;   //是否打印到终端
    string logName;             //日志文件名字
    string logFilePath;         //日志文件保存路径
    string logMixSize;          //日志文件最大大小
    string logBehavior;         //日志文件达到最大大小行为
    string logOverlay;          //日志文件覆盖时间
    string logOutputLevelFile;  //日志输出等级(file)
    string logOutputLevelTerminal;//日志输出等级
};

class FdogLogger {
public:
    void initLogConfig();

    void releaseConfig();

    static FdogLogger* getInstance();

    string getCoutType(coutType coutType);

    bool getFileType(fileType fileCoutBool);

    bool getTerminalType(terminalType terminalCoutTyle);

    string getLogCoutTime();

    string getLogNameTime();

    string getFilePash();

    string getLogCoutProcessId();

    string getLogCoutThreadId();

    string getLogCoutUserName();

    bool createFile(string filePash);

    bool logFileWrite(string messages);

    bool bindFileCoutMap(string value1, fileType value2);

    bool bindTerminalCoutMap(string value1, terminalType value2);

private:
    char szbuf[128];
    Logger logger;
    static FdogLogger * singleObject;
    static mutex * mutex_new;
    map<coutType, string> coutTypeMap;
    map<fileType, bool> fileCoutMap;
    map<terminalType, bool> terminalCoutMap;

private:
    FdogLogger();
    ~FdogLogger();
};

#define Error1 __FDOGNAME__(Error)
#define Warn1 __FDOGNAME__(Warn)
#define Info1 __FDOGNAME__(Info)
#define Debug1 __FDOGNAME__(Debug)
#define Trace1 __FDOGNAME__(Trace)


#define SQUARE_BRACKETS_LEFT " ["
#define SQUARE_BRACKETS_RIGHT "] "
#define SPACE " "
#define LINE_FEED "\n"
#define COLON ":"
#define SLASH "/"

#define __FDOGTIME__  FdogLogger::getInstance()->getLogCoutTime()          //时间宏
#define __FDOGPID__   FdogLogger::getInstance()->getLogCoutProcessId()     //进程宏
#define __FDOGTID__   FdogLogger::getInstance()->getLogCoutThreadId()      //线程宏
#define __FDOGFILE__  __FILE__        //文件名宏
#define __FDOGPASH__  FdogLogger::getInstance()->getFilePash() + __FDOGFILE__ //文件路径
#define __FDOGFUNC__   __func__        //函数名宏
#define __FDOGLINE__  __LINE__        //行数宏
#define __USERNAME__  FdogLogger::getInstance()->getLogCoutUserName()     //获取调用用户名字
#define __FDOGNAME__(name) #name        //名字宏


#define COMBINATION_INFO_FILE(coutTypeInfo, message) \
    do{\
        string messagesAll = __FDOGTIME__ + coutTypeInfo + __USERNAME__ + __FDOGTID__ + SQUARE_BRACKETS_LEFT + \
        __FDOGPASH__  + SPACE +__FDOGFUNC__ + COLON + to_string(__FDOGLINE__) + SQUARE_BRACKETS_RIGHT + message + LINE_FEED;\
        FdogLogger::getInstance()->logFileWrite(messagesAll); \
    }while(0);

#define COMBINATION_INFO_TERMINAL(coutTypeInfo, message) \
    do{\
        string messagesAll = __FDOGTIME__ + WHITE + coutTypeInfo + DEFA + __USERNAME__ + __FDOGTID__ + SQUARE_BRACKETS_LEFT + \
        __FDOGPASH__  + SPACE +__FDOGFUNC__ + COLON + to_string(__FDOGLINE__) + SQUARE_BRACKETS_RIGHT + message + LINE_FEED;\
        cout << messagesAll;\
    }while(0);

#define LoggerCout(coutTyle, coutTypeInfo, fileCoutBool, terminalCoutBool, message) \
    do {\
        string coutType = FdogLogger::getInstance()->getCoutType(coutTyle);\
        if (FdogLogger::getInstance()->getFileType(fileCoutBool)) {\
            COMBINATION_INFO_FILE(coutTypeInfo, message)\
        }\
        if (FdogLogger::getInstance()->getTerminalType(terminalCoutBool)) {\
            COMBINATION_INFO_TERMINAL(coutTypeInfo, message)\
        }\
    }while(0);

#define FdogError(message) \
    do{\
        LoggerCout(fdog::coutType::Error, Error1, fdog::fileType::Error, fdog::terminalType::Error, message)\
    }while(0);

#define FdogWarn(message)  \
    do{\
        LoggerCout(fdog::coutType::Warn, Warn1, fdog::fileType::Warn, fdog::terminalType::Warn, message)\
    }while(0);

#define FdogInfo(message)  \
    do{\
        LoggerCout(fdog::coutType::Info, Info1, fdog::fileType::Info, fdog::terminalType::Info, message)\
    }while(0);

#define FdogDebug(message) \
    do{\
        LoggerCout(fdog::coutType::Debug, Debug1, fdog::fileType::Debug, fdog::terminalType::Debug, message)\
    }while(0);

#define FdogTrace(message) \
    do{\
        LoggerCout(fdog::coutType::Trace, Trace1, fdog::fileType::Trace, fdog::terminalType::Trace, message)\
    }while(0);

}

#endif


