#include"filemanagement.h"
#define ONEDAY 24 * 60 * 60

FileManagement::FileManagement(){
}

FileManagement::~FileManagement(){
    //file.close();
}

bool FileManagement::createFilePash(string fileName){
    int len = fileName.length();
    if(!len){
        fileName = "log";
        if (0 != access(fileName.c_str(), 0)){
            if(-1 == mkdir(fileName.c_str(),0)){
                return 0;
            }
        }
    }
    std::string fileName_cy(len,'\0');
    for(int i =0;i<len;i++){
        fileName_cy[i]=fileName[i];
        if(fileName_cy[i]=='/' || fileName_cy[i]=='\\'){
            if (-1 == access(fileName_cy.c_str(), 0)){
                if(0!=mkdir(fileName_cy.c_str(),0)){
                    return 0;
                }
            }
        }
    }
    return 1;
}

bool FileManagement::createFile(string fileName) {
    ofstream file;
    file.open(fileName, ::ios::app | ios::out);
    if(!file) {
        cout<<"Failed to create file"<<endl;
        return 0;
    }
    file.close();
    return 1;
}

bool FileManagement::verifyFileExistence(string fileName){
    return (access(fileName.data(), F_OK) != -1);
}

bool FileManagement::verifyFileValidityDays(string fileName, string logOverlay){
    struct stat statbuf;
    timespec time_;
    if(stat(fileName.data(),&statbuf) == 0) {
        time_ = statbuf.st_mtim;
    }
    long logOverlay_i = (long)atoi(logOverlay.data());
    long nowtime = getCurrentTime();
    long difftime =  nowtime - time_.tv_sec;

    if ((logOverlay_i * ONEDAY) >= difftime) {
        return true;
    }
    return false;
}

bool FileManagement::fileRename(string oldFile, string newFile){
    if(!rename(oldFile.data(), newFile.data())) {
        cout<<"File rename failed"<<endl;
    }
    return 0;
}

long FileManagement::verifyFileSize(string fileName){
    struct stat statbuf;
    if(stat(fileName.data(),&statbuf) == 0) {
        return statbuf.st_size;
    }
    //int size = filelength(fileno(filePash));
    return -1;
}

long FileManagement::getCurrentTime(){
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    timep = mktime(p);
    return timep;
}