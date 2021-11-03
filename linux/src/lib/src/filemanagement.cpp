#ifdef _WIN32
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#endif

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
#ifdef __linux__
        if (0 != access(fileName.c_str(), F_OK)){
            if(-1 == mkdir(fileName.c_str(),0)){
                return false;
            }
        }
#elif _WIN32
		if (0 != access(fileName.c_str(), 0)) {
			if (-1 == mkdir(fileName.c_str())) {
				return false;
			}
		}
#endif
	}
    std::string fileName_cy(len,'\0');
    for(int i =0;i<len;i++){
        fileName_cy[i]=fileName[i];
        if(fileName_cy[i]=='/' || fileName_cy[i]=='\\'){
#ifdef __linux__
            if (-1 == access(fileName_cy.c_str(), F_OK)){
                if(0!=mkdir(fileName_cy.c_str(),0)){
                    return false;
                }
            }
#elif _WIN32
			if (-1 == access(fileName_cy.c_str(), 0)) {
				if (0 != mkdir(fileName_cy.c_str())) {
					return false;
				}
			}
#endif
        }
    }
    return true;
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
#ifdef __linux__
	return (access(fileName.data(), F_OK) != -1);
#elif _WIN32
	return (access(fileName.data(), 0) != -1);
#endif
}

//bool FileManagement::verifyFileValidityDays(string fileName, string logOverlay){
//#ifdef __linux__
//	struct stat statbuf;
//	timespec time_;
//	if (stat(fileName.data(), &statbuf) == 0) {
//		time_ = statbuf.st_mtim;
//	}
//#elif _WIN32
//	struct stat statbuf;
//	timespec time_;
//	if (stat(fileName.data(), &statbuf) == 0) {
//		time_ = statbuf.st_mtim;
//	}
//#endif
//    long logOverlay_i = (long)atoi(logOverlay.data());
//    long nowtime = getCurrentTime();
//    long difftime =  nowtime - time_.tv_sec;
//
//    if ((logOverlay_i * ONEDAY) >= difftime) {
//        return true;
//    }
//    return false;
//}

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