#include"fdoglogger.h"

using namespace fdog;


FdogLogger * FdogLogger::singleObject = nullptr;
mutex * FdogLogger::mutex_log = new(mutex);
mutex * FdogLogger::mutex_file = new(mutex);
mutex * mutex_terminal = new(mutex);

FdogLogger::FdogLogger(){
    initLogConfig();
}

FdogLogger::~FdogLogger(){

}

FdogLogger* FdogLogger::getInstance(){
    mutex_log->lock();
    if (singleObject == nullptr) {
        singleObject = new FdogLogger();
    }
    mutex_log->unlock();
    return singleObject;
}

void FdogLogger::initLogConfig(){

    coutColor["Error"] = "\e[1;31m";
    coutColor["Warn"] = "\e[1;35m";
    coutColor["Info"] = "\e[1;34m";
    coutColor["Debug"] = "\e[1;32m";
    coutColor["Trace"] = "\e[1;37m";

    map<string, string *> flogConfInfo;
    flogConfInfo["logSwitch"] = &this->logger.logSwitch;
    flogConfInfo["logFileSwitch"] = &this->logger.logFileSwitch;
    flogConfInfo["logTerminalSwitch"] = &this->logger.logTerminalSwitch;
    flogConfInfo["logName"] = &this->logger.logName;
    flogConfInfo["logFilePath"] = &this->logger.logFilePath;
    flogConfInfo["logMixSize"] = &this->logger.logMixSize;
    flogConfInfo["logBehavior"] = &this->logger.logBehavior;
    flogConfInfo["logOverlay"] = &this->logger.logOverlay;
    flogConfInfo["logOutputLevelFile"] = &this->logger.logOutputLevelFile;
    flogConfInfo["logOutputLevelTerminal"] = &this->logger.logOutputLevelTerminal;

    string str;
    ifstream file;
    char str_c[100]={0};
    file.open("../../conf/fdoglogconf.conf");
    if(!file.is_open()){
        cout<<"File open failed" <<endl;
    }
    while(getline(file, str)){
        if(!str.length()) {
            continue;
        }
        string str_copy = str;
        int j = 0;
        for(int i = 0; i < str.length(); i++){
            if(str[i]==' ')continue;
            str_copy[j] = str[i];
            j++;
        }
        str_copy.erase(j);
        if(str_copy[0]!='#'){
            sscanf(str_copy.data(),"%[^=]",str_c);
            auto iter = flogConfInfo.find(str_c);
            if(iter!=flogConfInfo.end()){
                sscanf(str_copy.data(),"%*[^=]=%s",str_c);
                *iter->second = str_c;
            } else {
            }
        }
    }
    file.close();

    bindFileCoutMap("5", fileType::Error);
    bindFileCoutMap("4", fileType::Warn);
    bindFileCoutMap("3", fileType::Info);
    bindFileCoutMap("2", fileType::Debug);
    bindFileCoutMap("1", fileType::Trace);

    bindTerminalCoutMap("5", terminalType::Error);
    bindTerminalCoutMap("4", terminalType::Warn);
    bindTerminalCoutMap("3", terminalType::Info);
    bindTerminalCoutMap("2", terminalType::Debug);
    bindTerminalCoutMap("1", terminalType::Trace);

    string filePashAndName = getFilePathAndName();
    string filePash = getFilePash();
    if(logger.logFileSwitch == "on"){
        //检查路径
        filemanagement.createFilePash(filePash);
        //检测文件有效性
        if(!filemanagement.verifyFileExistence(filePashAndName)) {
            filemanagement.createFile(filePashAndName);

        } else {
            if(!filemanagement.verifyFileValidityDays(filePashAndName, logger.logOverlay)){
                string newFileName = getFilePathAndNameAndTime();
                filemanagement.fileRename(filePashAndName, newFileName);

                filemanagement.createFile(filePashAndName);
            } else {
                long fileSize = filemanagement.verifyFileSize(filePashAndName);
                if (fileSize > (long)atoi(logger.logMixSize.data()) * 1048576 && logger.logBehavior == "1"){
                    string newFileName = getFilePathAndNameAndTime();
                    filemanagement.fileRename(filePashAndName, newFileName);
                    filemanagement.createFile(filePashAndName);
                }
            }
        }
    }  
    return;
}

void FdogLogger::ConfInfoPrint() {
    // cout << "|========FdogLogger v2.0==========================|" <<endl << endl;
    // cout << "  日志开关：" << logger.logSwitch << endl;
    // cout << "  文件输出：" << logger.logFileSwitch << endl;
    // cout << "  终端输出：" << logger.logTerminalSwitch << endl;
    // cout << "  日志输出等级(文件)：" << logger.logOutputLevelFile << endl;    
    // cout << "  日志输出等级(终端)：" << logger.logOutputLevelTerminal << endl;
    // cout << "  日志文件名：" << logger.logName << endl;
    // cout << "  日志保存路径：" << logger.logFilePath << endl;
    // cout << "  单文件最大大小："<< logger.logMixSize << "M" << endl;
    // cout << "  日志保存时间 ：" << logger.logOverlay << "天" << endl << endl;
    // cout << "|=================================================|" <<endl;
}

string FdogLogger::getCoutType(coutType coutType){
    return singleObject->coutTypeMap[coutType];
}

bool FdogLogger::getFileType(fileType fileCoutBool){
    return singleObject->fileCoutMap[fileCoutBool];
}

bool FdogLogger::getTerminalType(terminalType terminalCoutTyle){
    return singleObject->terminalCoutMap[terminalCoutTyle];
}

string FdogLogger::getLogCoutTime(){
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep));
    string tmp_str = tmp;

    return SQUARE_BRACKETS_LEFT + tmp_str + SQUARE_BRACKETS_RIGHT;
}

string FdogLogger::getLogNameTime(){
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d-%H:%M:%S",localtime(&timep));
    return tmp;
}

string FdogLogger::getSourceFilePash(){
    getcwd(szbuf, sizeof(szbuf)-1);
    string szbuf_str = szbuf;
    return szbuf_str + SLASH;
}
string FdogLogger::getFilePash(){
    return logger.logFilePath + SLASH;
}

string FdogLogger::getFilePathAndName(){
    return logger.logFilePath + SLASH + logger.logName + ".log";
}

string FdogLogger::getFilePathAndNameAndTime(){
    return logger.logFilePath + logger.logName + getLogNameTime() + ".log";
}

string FdogLogger::getLogCoutProcessId(){
#ifndef linux
    return to_string(getpid());
#endif
#ifndef WIN32
//  unsigned long GetPid(){
//     return GetCurrentProcessId();
// }
#endif
}

string FdogLogger::getLogCoutThreadId(){
#ifndef linux
    return to_string(syscall(__NR_gettid));
#endif
#ifndef WIN32
//  unsigned long GetTid(){
//     return GetCurrentThreadId();
// }
#endif
}

string FdogLogger::getLogCoutUserName(){
    struct passwd *my_info;
    my_info = getpwuid(getuid());
    string name = my_info->pw_name;
    return SPACE + name + SPACE;
}

bool FdogLogger::logFileWrite(string messages, string message, string line_effd){
    string filePashAndName = getFilePathAndName();

    long fileSize = filemanagement.verifyFileSize(filePashAndName);
    if (fileSize > (long)atoi(logger.logMixSize.data()) * 1048576 && logger.logBehavior == "1"){
        string newFileName = getFilePathAndNameAndTime();
        filemanagement.fileRename(filePashAndName, newFileName);
        filemanagement.createFile(filePashAndName);
    }
    mutex_file->lock();
    ofstream file;
    file.open(filePashAndName, ::ios::app | ios::out);
    //这里应该用队列，缓冲区达到一定量再写入，频繁打开文件不太好，先这样吧
    file << messages << message << line_effd;
    file.close();
    mutex_file->unlock();
    return 1;
}

string FdogLogger::getLogFileSwitch(){
    return logger.logFileSwitch;
}

string FdogLogger::getLogTerminalSwitch(){
    return logger.logTerminalSwitch;
}
string FdogLogger::getCoutTypeColor(string colorType){
    return coutColor[colorType];
}

bool FdogLogger::bindFileCoutMap(string value1, fileType value2){
    if(logger.logOutputLevelFile.find(value1)!=std::string::npos) {
        fileCoutMap[value2] = true;
    } else {
        fileCoutMap[value2] = false;
    }
}

bool FdogLogger::bindTerminalCoutMap(string value1, terminalType value2){
    if(logger.logOutputLevelTerminal.find(value1)!=std::string::npos) {
        terminalCoutMap[value2] = true;
    } else {
        terminalCoutMap[value2] = false;
    }
}