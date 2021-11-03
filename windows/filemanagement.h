#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H
#include<iostream>
#include<string>
#include<fstream>
#include<time.h>
#ifdef __linux__
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<sys/types.h>
#elif _WIN32
#include<io.h>
#include<direct.h>
#endif
using namespace std;

class FileManagement {
public:
    FileManagement();

    ~FileManagement();

    bool createFilePash(string fileName);

    bool createFile(string fileName);

    bool verifyFileExistence(string fileName);

    //bool verifyFileValidityDays(string fileName, string logOverlay);废除

    bool fileRename(string oldFile, string newFile);

    long verifyFileSize(string fileName);

    long getCurrentTime();

};

#endif