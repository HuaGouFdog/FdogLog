#ifdef _WIN32
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#endif
#include"fdoglogger.h"

using namespace fdog;

FdogLogger * FdogLogger::singleObject = nullptr;
mutex * FdogLogger::mutex_log = new(mutex);
mutex * FdogLogger::mutex_file = new(mutex);
mutex * FdogLogger::mutex_queue = new(mutex);
mutex * FdogLogger::mutex_terminal = new(mutex);

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
#ifdef __linux__
    coutColor["Error"] = "\e[1;31m";
    coutColor["Warn"] = "\e[1;35m";
    coutColor["Info"] = "\e[1;34m";
    coutColor["Debug"] = "\e[1;32m";
    coutColor["Trace"] = "\e[1;37m";
#elif _WIN32
	coutColor["Error"] = "";
	coutColor["Warn"] = "";
	coutColor["Info"] = "";
	coutColor["Debug"] = "";
	coutColor["Trace"] = "";
#endif

    map<string, string *> flogConfInfo;
    flogConfInfo["logSwitch"] = &this->logger.logSwitch;
    flogConfInfo["logFileSwitch"] = &this->logger.logFileSwitch;
    flogConfInfo["logTerminalSwitch"] = &this->logger.logTerminalSwitch;
    flogConfInfo["logFileQueueSwitch"] = &this->logger.logFileQueueSwitch;
    flogConfInfo["logName"] = &this->logger.logName;
    flogConfInfo["logFilePath"] = &this->logger.logFilePath;
    flogConfInfo["logMixSize"] = &this->logger.logMixSize;
    flogConfInfo["logBehavior"] = &this->logger.logBehavior;
    flogConfInfo["logOutputLevelFile"] = &this->logger.logOutputLevelFile;
    flogConfInfo["logOutputLevelTerminal"] = &this->logger.logOutputLevelTerminal;
	
	bool isOpen = true;
    string str;
    ifstream file;
    char str_c[100]={0};
#ifdef __linux__
    file.open("../../conf/fdoglogconf.conf");
#elif _WIN32
	file.open("fdoglogconf.conf");
#endif
    if(!file.is_open()){
		isOpen = false;
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
	cout << filePashAndName << " : " << filePash << endl;
    if(logger.logFileSwitch == SWITCH_ON){
        //检查路径
        filemanagement.createFilePash(filePash);
        //检测文件有效性
        if(!filemanagement.verifyFileExistence(filePashAndName)) {
            filemanagement.createFile(filePashAndName);
        } else {
            long fileSize = filemanagement.verifyFileSize(filePashAndName);
            if (fileSize > (long)atoi(logger.logMixSize.data()) * MEGABYTES && logger.logBehavior == "1"){
                string newFileName = getFilePathAndNameAndTime();
                filemanagement.fileRename(filePashAndName, newFileName);
                filemanagement.createFile(filePashAndName);
            }
        }
    }
	if (isOpen) {
		ConfInfoPrint();
	}
    return;
}

void FdogLogger::releaseConfig() {

}

void FdogLogger::ConfInfoPrint() {
#ifdef __linux__
    for(int i = 0; i < logger.logFilePath.size() + 15; i++){
        cout << GREEN << "-";
        if(i == (logger.logFilePath.size() + 15)/2){
            cout << "FdogLogger";
        }
    }
    cout << DEFA << endl;
    cout << GREEN << ::left<<setw(25) << "  日志开关　　" << logger.logSwitch << DEFA << endl;
    cout << GREEN << ::left<<setw(25) << "  文件输出　　" << logger.logFileSwitch << DEFA << endl;
    cout << GREEN << ::left<<setw(25) << "  终端输出开关" << logger.logTerminalSwitch << DEFA << endl;
    cout << GREEN << ::left<<setw(25) << "  文件输出等级" << logger.logOutputLevelFile << DEFA << endl;    
    cout << GREEN << ::left<<setw(25) << "  终端输出等级" << logger.logOutputLevelTerminal << DEFA << endl;
    cout << GREEN << ::left<<setw(25) << "  日志队列策略" << logger.logFileQueueSwitch << DEFA << endl;    
    cout << GREEN << ::left<<setw(25) << "  日志文件名称" << logger.logName << ".log" << DEFA << endl;
    cout << GREEN << ::left<<setw(25) << "  日志保存路径" << logger.logFilePath << DEFA << endl;
    cout << GREEN << ::left<<setw(25) << "  日志文件大小" << logger.logMixSize << "M" << DEFA << endl;
    for(int i = 0; i < logger.logFilePath.size() + 25; i++){
        cout << GREEN << "-" << DEFA;
    }
    cout << endl;
#elif _WIN32
	for (int i = 0; i < logger.logFilePath.size() + 15; i++) {
		cout << "-";
		if (i == (logger.logFilePath.size() + 15) / 2) {
			cout << "FdogLogger";
		}
	}
	cout << endl;
	cout << ::left << setw(25) << "  日志开关　　" << logger.logSwitch << endl;
	cout << ::left << setw(25) << "  文件输出　　" << logger.logFileSwitch << endl;
	cout << ::left << setw(25) << "  终端输出开关" << logger.logTerminalSwitch << endl;
	cout << ::left << setw(25) << "  文件输出等级" << logger.logOutputLevelFile << endl;
	cout << ::left << setw(25) << "  终端输出等级" << logger.logOutputLevelTerminal << endl;
	cout << ::left << setw(25) << "  日志队列策略" << logger.logFileQueueSwitch << endl;
	cout << ::left << setw(25) << "  日志文件名称" << logger.logName << ".log" << endl;
	cout << ::left << setw(25) << "  日志保存路径" << logger.logFilePath << endl;
	cout << ::left << setw(25) << "  日志文件大小" << logger.logMixSize << "M" << endl;
	for (int i = 0; i < logger.logFilePath.size() + 25; i++) {
		cout << "-";
	}
	cout << endl;
#endif
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
#ifdef __linux__
	getcwd(szbuf, sizeof(szbuf) - 1);
#elif _WIN32
	getcwd(szbuf, sizeof(szbuf) - 1);
#endif
    string szbuf_str = szbuf;
    return szbuf_str + SLASH;
}
string FdogLogger::getFilePash(){
    return logger.logFilePath + SLASH;
}

string FdogLogger::getFilePathAndName(){
#ifdef __linux__
    return logger.logFilePath + SLASH + logger.logName + ".log";
#elif _WIN32
	return logger.logFilePath + logger.logName + ".log";
#endif
}

string FdogLogger::getFilePathAndNameAndTime(){
    return logger.logFilePath + logger.logName + getLogNameTime() + ".log";
}

string FdogLogger::getLogCoutProcessId(){
#ifdef __linux__
    return to_string(getpid());
#elif _WIN32
	return to_string(getpid());
//  return GetCurrentProcessId();
#endif
}

string FdogLogger::getLogCoutThreadId(){
#ifdef __linux__
    return to_string(syscall(__NR_gettid));
#elif _WIN32
    return to_string(GetCurrentThreadId());
#endif

}

string FdogLogger::getLogCoutUserName(){
#ifdef __linux__
	struct passwd * my_info;
	my_info = getpwuid(getuid());
	string name = my_info->pw_name;
	return SPACE + name + SPACE;
#elif _WIN32
	const int MAX_LEN = 100;
	TCHAR szBuffer[MAX_LEN];
	DWORD len = MAX_LEN;
	GetUserName(szBuffer, &len);
	
	int iLen = WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, NULL, 0, NULL, NULL);
	char * chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, chRtn, iLen, NULL, NULL);
	string str(chRtn);
	return " " + str + " ";
#endif

}

bool FdogLogger::logFileWrite(string messages, string message, string line_effd){
    string filePashAndName = getFilePathAndName();

    long fileSize = filemanagement.verifyFileSize(filePashAndName);
    if (fileSize > (long)atoi(logger.logMixSize.data()) * MEGABYTES && logger.logBehavior == "1"){
        string newFileName = getFilePathAndNameAndTime();
        filemanagement.fileRename(filePashAndName, newFileName);
        filemanagement.createFile(filePashAndName);
    }
    if(logger.logFileQueueSwitch == SWITCH_OFF){
        mutex_file->lock();
        ofstream file;
        file.open(filePashAndName, ::ios::app | ios::out);
        file << messages << message << line_effd;
        file.close();
        mutex_file->unlock();
    }else{
        insertQueue(messages + message + line_effd, filePashAndName);
    }
    return 1;
}

bool FdogLogger::insertQueue(string messages, string filePashAndName){
    mutex_queue->lock();
    messageQueue.push(messages);
    if(messageQueue.size() >= 5000){
        mutex_file->lock();
        ofstream file;
        file.open(filePashAndName, ::ios::app | ios::out);
        while(!messageQueue.empty()){
            file << messageQueue.front();
            messageQueue.pop();
        }
        file.close();
        mutex_file->unlock();
    }
    mutex_queue->unlock();
	return true;
}

string FdogLogger::getLogSwitch(){
    return logger.logSwitch;
}

string FdogLogger::getLogFileSwitch(){
    return logger.logFileSwitch;
}

string FdogLogger::getLogTerminalSwitch(){
    return logger.logTerminalSwitch;
}
string FdogLogger::getCoutTypeColor(string colorType){
#ifdef __linux__
	return coutColor[colorType];
#elif _WIN32
	return "";
#endif
}

bool FdogLogger::bindFileCoutMap(string value1, fileType value2){
    if(logger.logOutputLevelFile.find(value1) != std::string::npos) {
        fileCoutMap[value2] = true;
    } else {
        fileCoutMap[value2] = false;
    }
	return true;
}

bool FdogLogger::bindTerminalCoutMap(string value1, terminalType value2){
    if(logger.logOutputLevelTerminal.find(value1)!=std::string::npos) {
        terminalCoutMap[value2] = true;
    } else {
        terminalCoutMap[value2] = false;
    }
	return true;
}
