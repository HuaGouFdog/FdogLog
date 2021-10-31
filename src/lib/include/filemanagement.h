#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H
#include<iostream>
#include<string>
#include<fstream>
#ifndef linux
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/stat.h>
#include<sys/types.h>
#endif
using namespace std;

class FileManagement {
public:
    FileManagement();

    ~FileManagement();

    bool createFilePash(string fileName);

    bool createFile(string fileName);

    bool verifyFileExistence(string fileName);

    bool verifyFileValidityDays(string fileName, string logOverlay);

    bool fileRename(string oldFile, string newFile);

    long verifyFileSize(string fileName);

    long getCurrentTime();

};

#endif