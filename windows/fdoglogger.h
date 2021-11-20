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

namespace fdog {

#define Error1 __FDOGNAME__(Error)
#define Warn1 __FDOGNAME__(Warn)
#define Info1 __FDOGNAME__(Info)
#define Debug1 __FDOGNAME__(Debug)
#define Trace1 __FDOGNAME__(Trace)

#define __FDOGTIME__  FdogLogger::getInstance()->getLogCoutTime()          //时间宏
#define __FDOGPID__   FdogLogger::getInstance()->getLogCoutProcessId()     //进程宏
#define __FDOGTID__   FdogLogger::getInstance()->getLogCoutThreadId()      //线程宏
#define __USERNAME__  FdogLogger::getInstance()->getLogCoutUserName()     //获取调用用户名字
#define __FDOGFILE__  __FILE__          //文件名宏
#define __FDOGFUNC__  __func__          //函数名宏
#define __FDOGLINE__  __LINE__          //行数宏
#define __FDOGNAME__(name) #name        //名字宏

const int MEGABYTES = 1048576;
const std::string  SQUARE_BRACKETS_LEFT  = " [";
const std::string  SQUARE_BRACKETS_RIGHT = "] ";
const std::string SPACE = " ";
const std::string LINE_FEED = "\n";
const std::string COLON  = ":";
const std::string SLASH = "/";
const std::string DEFA = "\e[0m";
const std::string SWITCH_OFF = "off";
const std::string SWITCH_ON = "on";
const std::string RED = "\e[1;31m";
const std::string BLUE = "\e[1;34m";
const std::string GREEN = "\e[1;32m";
const std::string WHITE = "\e[1;37m";
const std::string PURPLE = "\e[1;35m";

enum class coutType: int {Error, Warn, Info, Debug, Trace};
enum class fileType: int {Error, Warn, Info, Debug, Trace};
enum class terminalType: int {Error, Warn, Info, Debug, Trace};

struct Logger {
    std::string logSwitch;           //日志开关
    std::string logFileSwitch;       //是否写入文件
    std::string logTerminalSwitch;   //是否打印到终端
    std::string logFileQueueSwitch;  //是否开启队列策略
    std::string logName;             //日志文件名字
    std::string logFilePath;         //日志文件保存路径
    std::string logMixSize;          //日志文件最大大小
    std::string logBehavior;         //日志文件达到最大大小行为
    std::string logOutputLevelFile;  //日志输出等级(file)
    std::string logOutputLevelTerminal;//日志输出等级
};

class FdogLogger {
private:
    char sourceFilePash[128];
    Logger logger;
    static FdogLogger * singleObject;
    static std::mutex * mutex_log;
    static std::mutex * mutex_file;
    static std::mutex * mutex_queue;
    std::map<coutType, std::string> coutTypeMap;
    std::map<fileType, bool> fileCoutMap;
    std::map<terminalType, bool> terminalCoutMap;
    std::map<std::string, std::string> coutColor;
    std::queue<std::string> messageQueue;

private:
    FdogLogger();
    ~FdogLogger();

public:
    static std::mutex * mutex_terminal;

    void initLogConfig();

    void releaseConfig();

    void ConfInfoPrint();

    static FdogLogger* getInstance();

    const std::string getLogCoutTime()const;

    const std::string getLogNameTime()const;

    const std::string getSourceFilePash();

    const std::string getFilePash()const;

    const std::string getFilePathAndName()const;

    const std::string getFilePathAndNameAndTime()const;

    const std::string getLogCoutProcessId()const;

    const std::string getLogCoutThreadId()const;

    const std::string getLogCoutUserName()const;
    
    const std::string getLogSwitch()const;

    const std::string getLogFileSwitch()const;

    const std::string getLogTerminalSwitch()const;

    const std::string getCoutType(const coutType &coutType)const;

    const std::string getCoutTypeColor(const std::string &colorType);

    const bool getFileType(const fileType &fileCoutBool);

    const bool getTerminalType(const terminalType &terminalCoutTyle);

    const bool logFileWrite(const std::string &messages, const std::string &message, const std::string &LINE);

    const bool insertQueue(const std::string &messages, const std::string &filePashAndName);

    const bool bindFileCoutMap(const std::string &value1, const fileType &value2);

    const bool bindTerminalCoutMap(const std::string &value1, const terminalType &value2);
};

#define KV(value) " " << #value << "=" << value

#define COMBINATION_INFO_FILE(coutTypeInfo, message) \
    do{\
        std::ostringstream oss;\
        std::streambuf* pOldBuf = std::cout.rdbuf(oss.rdbuf());\
        std::cout << message;\
        std::string ret = oss.str();\
        std::cout.rdbuf(pOldBuf);\
        std::string messagesAll = __FDOGTIME__ + coutTypeInfo + __USERNAME__ + __FDOGTID__ + SQUARE_BRACKETS_LEFT + \
        __FDOGFILE__  + SPACE +__FDOGFUNC__ + COLON + std::to_string(__FDOGLINE__) + SQUARE_BRACKETS_RIGHT;\
        FdogLogger::getInstance()->logFileWrite(messagesAll, ret, LINE_FEED); \
    }while(0);

#ifdef __linux__
#define COMBINATION_INFO_TERMINAL(coutTypeInfo, message) \
    do{\
        std::string color = FdogLogger::getInstance()->getCoutTypeColor(coutTypeInfo);\
        std::string logFormatCout = __FDOGTIME__ + color + coutTypeInfo + DEFA + __USERNAME__ + __FDOGTID__ + SQUARE_BRACKETS_LEFT + \
        __FDOGFILE__  + SPACE +__FDOGFUNC__ + COLON + std::to_string(__FDOGLINE__) + SQUARE_BRACKETS_RIGHT;\
		FdogLogger::mutex_terminal->lock(); \
        std::cout << logFormatCout << message << LINE_FEED;\
		fflush(stdout);\
        FdogLogger::mutex_terminal->unlock(); \
    }while(0);
#elif _WIN32
    do{\
        std::string logFormatCout = __FDOGTIME__ + coutTypeInfo +  __USERNAME__ + __FDOGTID__ + SQUARE_BRACKETS_LEFT + \
        __FDOGFILE__  + SPACE +__FDOGFUNC__ + COLON + std::to_string(__FDOGLINE__) + SQUARE_BRACKETS_RIGHT;\
		FdogLogger::mutex_terminal->lock(); \
        std::cout << logFormatCout << message << LINE_FEED;\
		fflush(stdout);\
        FdogLogger::mutex_terminal->unlock(); \
    }while(0);
#endif

#define LoggerCout(coutTyle, coutTypeInfo, fileCoutBool, terminalCoutBool, message) \
    do {\
        std::string coutType = FdogLogger::getInstance()->getCoutType(coutTyle);\
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


