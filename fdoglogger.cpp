#include"fdoglogger.h"

using namespace fdog;


FdogLogger * FdogLogger::singleObject = nullptr;
mutex * FdogLogger::mutex_new = new(mutex);

FdogLogger::FdogLogger(){
    initLogConfig();
}
FdogLogger::~FdogLogger(){

}

FdogLogger* FdogLogger::getInstance(){
    mutex_new->lock();
    if (singleObject == nullptr) {
        singleObject = new FdogLogger();
    }
    mutex_new->unlock();
    return singleObject;
}

void FdogLogger::initLogConfig(){

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
    file.open("fdoglogconf.conf");
    if(!file.is_open()){
        cout<<"文件打开失败\n";
    }
    while(getline(file, str)){
        if(!str.length()) {
            continue;
        }
        string str_copy = str;
        //cout<<"获取数据："<<str_copy<<endl;
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
    logger.logName = logger.logName + getLogNameTime() + ".log";

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

    if(logger.logFileSwitch == "on"){
        if(!createFile(logger.logFilePath)){
            std::cout<<"Log work path creation failed\n";
        }
    }

    cout << "|========FdogLogger v2.0==========================|" <<endl << endl;
    cout << "  日志开关：" << logger.logSwitch << endl;
    cout << "  文件输出：" << logger.logFileSwitch << endl;
    cout << "  终端输出：" << logger.logTerminalSwitch << endl;
    cout << "  日志输出等级(文件)：" << logger.logOutputLevelFile << endl;    
    cout << "  日志输出等级(终端)：" << logger.logOutputLevelTerminal << endl;
    cout << "  日志文件名：" << logger.logName << endl;
    cout << "  日志保存路径：" << logger.logFilePath << endl;
    cout << "  单文件最大大小："<< logger.logMixSize << "M" << endl;
    cout << "  日志保存时间 ：" << logger.logOverlay << "天" << endl << endl;
    cout << "|=================================================|" <<endl;
    
    return;
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

string FdogLogger::getFilePash(){
    getcwd(szbuf, sizeof(szbuf)-1);
    string szbuf_str = szbuf;
    return szbuf_str + SLASH;
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

bool FdogLogger::createFile(string filePash){
    int len = filePash.length();
    if(!len){
        filePash = "log";
        if (0 != access(filePash.c_str(), 0)){
            if(-1 == mkdir(filePash.c_str(),0)){
                std::cout<<"没路径";
                return 0;
            }
        }
    }
    std::string filePash_cy(len,'\0');
    for(int i =0;i<len;i++){
        filePash_cy[i]=filePash[i];
        if(filePash_cy[i]=='/' || filePash_cy[i]=='\\'){
            if (-1 == access(filePash_cy.c_str(), 0)){
                if(0!=mkdir(filePash_cy.c_str(),0)){
                    std::cout<<"有路径";
                    return 0;
                }
            }
        }
    }
    return 1;
}

bool FdogLogger::logFileWrite(string messages){
    ofstream file;
    file.open(logger.logFilePath + logger.logName, ::ios::app | ios::out);
    if(!file){
        cout<<"写失败"<<endl;
        return 0;
    }
    file << messages;
    file.close();
    return 1;
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