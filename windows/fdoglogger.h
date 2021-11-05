#ifndef FDOGLOGGER_H
#define FDOGLOGGER_H
#include<iostream>
#include<fstream>
#include <sstream>
#include<iomanip>
#include<map>
#include<mutex>
#include<queue>
#include"filemanagement.h"
#ifdef __linux__
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <pwd.h>
#elif _WIN32
#include<direct.h>
#include<process.h>
#include<Windows.h>
#endif

using namespace std;

namespace fdog {

#define RED   "\e[1;31m"
#define BLUE  "\e[1;34m"
#define GREEN "\e[1;32m"
#define WHITE "\e[1;37m"
#define PURPLE "\e[1;35m"
#define DEFA  "\e[0m"

#define MEGABYTES 1048576
#define SWITCH_OFF "off"
#define SWITCH_ON  "on"

enum class coutType: int {Error, Warn, Info, Debug, Trace};
enum class fileType: int {Error, Warn, Info, Debug, Trace};
enum class terminalType: int {Error, Warn, Info, Debug, Trace};

struct Logger {
    string logSwitch;           //日志开关
    string logFileSwitch;       //是否写入文件
    string logTerminalSwitch;   //是否打印到终端
    string logFileQueueSwitch;  //是否开启队列策略
    string logName;             //日志文件名字
    string logFilePath;         //日志文件保存路径
    string logMixSize;          //日志文件最大大小
    string logBehavior;         //日志文件达到最大大小行为
    string logOutputLevelFile;  //日志输出等级(file)
    string logOutputLevelTerminal;//日志输出等级
};

class FdogLogger {
public:
    void initLogConfig();

    void releaseConfig();

    void ConfInfoPrint();

    static FdogLogger* getInstance();

    string getCoutType(coutType coutType);

    bool getFileType(fileType fileCoutBool);

    bool getTerminalType(terminalType terminalCoutTyle);

    string getLogCoutTime();

    string getLogNameTime();

    string getSourceFilePash();

    string getFilePash();

    string getFilePathAndName();

    string getFilePathAndNameAndTime();

    string getLogCoutProcessId();

    string getLogCoutThreadId();

    string getLogCoutUserName();
    
    string getLogSwitch();

    string getLogFileSwitch();

    string getLogTerminalSwitch();

    string getCoutTypeColor(string colorType);

    bool logFileWrite(string messages, string message, string LINE);

    bool insertQueue(string messages, string filePashAndName);

    bool bindFileCoutMap(string value1, fileType value2);

    bool bindTerminalCoutMap(string value1, terminalType value2);

	static mutex * mutex_terminal;
private:
    char szbuf[128];
    Logger logger;
    FileManagement filemanagement;
    static FdogLogger * singleObject;
    static mutex * mutex_log;
    static mutex * mutex_file;
    static mutex * mutex_queue;
    map<coutType, string> coutTypeMap;
    map<fileType, bool> fileCoutMap;
    map<terminalType, bool> terminalCoutMap;
    map<string, string> coutColor;
    queue<string> messageQueue;

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
#define __USERNAME__  FdogLogger::getInstance()->getLogCoutUserName()     //获取调用用户名字
#define __FDOGFILE__  __FILE__          //文件名宏
#define __FDOGFUNC__  __func__          //函数名宏
#define __FDOGLINE__  __LINE__          //行数宏
#define __FDOGNAME__(name) #name        //名字宏


#define COMBINATION_INFO_FILE(coutTypeInfo, message) \
    do{\
        ostringstream oss;\
        streambuf* pOldBuf = std::cout.rdbuf(oss.rdbuf());\
        cout << message;\
        string ret = oss.str();\
        cout.rdbuf(pOldBuf);\
        string messagesAll = __FDOGTIME__ + coutTypeInfo + __USERNAME__ + __FDOGTID__ + SQUARE_BRACKETS_LEFT + \
        __FDOGFILE__  + SPACE +__FDOGFUNC__ + COLON + to_string(__FDOGLINE__) + SQUARE_BRACKETS_RIGHT;\
        FdogLogger::getInstance()->logFileWrite(messagesAll, ret, LINE_FEED); \
    }while(0);
    
#ifdef __linux__
#define COMBINATION_INFO_TERMINAL(coutTypeInfo, message) \
    do{\
        string color = FdogLogger::getInstance()->getCoutTypeColor(coutTypeInfo);\
        string logFormatCout = __FDOGTIME__ + color + coutTypeInfo + DEFA + __USERNAME__ + __FDOGTID__ + SQUARE_BRACKETS_LEFT + \
        __FDOGFILE__  + SPACE +__FDOGFUNC__ + COLON + to_string(__FDOGLINE__) + SQUARE_BRACKETS_RIGHT;\
		FdogLogger::mutex_terminal->lock(); \
        cout << logFormatCout << message << LINE_FEED;\
		fflush(stdout);\
        FdogLogger::mutex_terminal->unlock(); \
    }while(0);
#elif _WIN32
    do{\
        string color = FdogLogger::getInstance()->getCoutTypeColor(coutTypeInfo);\
        string logFormatCout = __FDOGTIME__ + color + coutTypeInfo +  __USERNAME__ + __FDOGTID__ + SQUARE_BRACKETS_LEFT + \
        __FDOGFILE__  + SPACE +__FDOGFUNC__ + COLON + to_string(__FDOGLINE__) + SQUARE_BRACKETS_RIGHT;\
		FdogLogger::mutex_terminal->lock(); \
        cout << logFormatCout << message << LINE_FEED;\
		fflush(stdout);\
        FdogLogger::mutex_terminal->unlock(); \
    }while(0);
#endif

#define LoggerCout(coutTyle, coutTypeInfo, fileCoutBool, terminalCoutBool, message) \
    do {\
        string coutType = FdogLogger::getInstance()->getCoutType(coutTyle);\
        if( SWITCH_ON == FdogLogger::getInstance()->getLogSwitch()){\
            if (SWITCH_OFF != FdogLogger::getInstance()->getLogFileSwitch()){\
                if (FdogLogger::getInstance()->getFileType(fileCoutBool)) {\
                    COMBINATION_INFO_FILE(coutTypeInfo, message)\
                }\
            }\
            if (SWITCH_OFF != FdogLogger::getInstance()->getLogTerminalSwitch()){\
                if (FdogLogger::getInstance()->getTerminalType(terminalCoutBool)) {\
                    COMBINATION_INFO_TERMINAL(coutTypeInfo, message)\
                }\
            }\
        }\
    }while(0);

#define FdogError(...) \
    do{\
        LoggerCout(fdog::coutType::Error, Error1, fdog::fileType::Error, fdog::terminalType::Error, __VA_ARGS__)\
    }while(0);

#define FdogWarn(...)  \
    do{\
        LoggerCout(fdog::coutType::Warn, Warn1, fdog::fileType::Warn, fdog::terminalType::Warn, __VA_ARGS__)\
    }while(0);

#define FdogInfo(...)  \
    do{\
        LoggerCout(fdog::coutType::Info, Info1, fdog::fileType::Info, fdog::terminalType::Info, __VA_ARGS__)\
    }while(0);

#define FdogDebug(...) \
    do{\
        LoggerCout(fdog::coutType::Debug, Debug1, fdog::fileType::Debug, fdog::terminalType::Debug, __VA_ARGS__)\
    }while(0);

#define FdogTrace(...) \
    do{\
        LoggerCout(fdog::coutType::Trace, Trace1, fdog::fileType::Trace, fdog::terminalType::Trace, __VA_ARGS__)\
    }while(0);

}

#endif


