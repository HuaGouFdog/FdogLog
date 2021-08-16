//日志类
#ifndef FDOGLOG_H
#define FDOGLOG_H
#include<iostream>
#include<time.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fstream>


#define RED   "\e[1;31m"
#define BLUE  "\e[1;34m"
#define GREEN "\e[1;32m"
#define WHITE "\e[1;37m"
#define DEFA  "\e[0m"

static bool isfile = false;                     //是否开始日志保存
static std::string fiileurl = "log";            //日志保存地址
static std::string filename = "fdoglog.log";    //文件名字
static int log_output_level = 5;                //输出等级

/*获取当前时间*/
static std::string GetDateTime()
{
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}
/*创建文件夹以及文件名*/
static bool CreateFile( std::string fileurl_s)
{
    if(fileurl_s=="")
    {
        fileurl_s = "log";
        if (0 != access(fileurl_s.c_str(), 0))
        {
            if(-1 == mkdir(fileurl_s.c_str(),0))   // 返回 0 表示创建成功，-1 表示失败
            {
                return 0;
            }
        }
    }
    else
    {
        int len = fileurl_s.length();
        std::string fileurl_s_cy(len,'\0');
        for(int i =0;i<len;i++)
        {
            fileurl_s_cy[i]=fileurl_s[i];
            if(fileurl_s_cy[i]=='/' || fileurl_s_cy[i]=='\\')
            {
                if (-1 == access(fileurl_s_cy.c_str(), 0))
                {
                    if(0!=mkdir(fileurl_s_cy.c_str(),0))
                    {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

/*写日志*/
static bool WriteLog(const std::string data)
{  
    //在此写入文件
    std::ofstream file;
    file.open(fiileurl+filename, std::ios::app | std::ios::out);
    if(!file)
    {
        return 0;
    }
    else
    {
        file<<data;
        file.close();
    }
    return 1;
}

/*日志保存机制*/
#define RECORD_LOG(isfile_b,fileurl_s,filename_s) do{\
isfile=isfile_b;\
fiileurl=fileurl_s;\
filename=filename_s;\
if(!CreateFile(fileurl_s))LOG_Error("Log work path creation failed");\
}while(0)

/*日志输出机制*/
#define Log_Output_Level(level) do{\
log_output_level = level;\
}while(0)

//标准库已有 __FILE__源文件名宏  __func__函数名宏  __LINE__ 行数宏
#define __FTIME__ GetDateTime() //自定义时间宏

/*ERROR（错误）:此信息输出后，主体系统核心模块不能正常工作，需要修复才能正常工作。*/
#define LOG_Error(text) do{\
std::string data = __FTIME__+RED+" ERROR "+DEFA+__FILE__+":"+__func__+"(line:"+std::to_string(__LINE__)+") "+text+"\n";\
if(isfile) if(!WriteLog(data))\
std::cout<<__FTIME__+" ERROR "+__FILE__+":"+__func__+":"+std::to_string(__LINE__)+" Log file write failed\n";\
if(log_output_level>0)std::cout<<data;\
}while(0)

/*WARN（警告）:此信息输出后，系统一般模块存在问题，不影响系统运行。*/
#define LOG_Warn(text) do{\
std::string data = __FTIME__+BLUE+" WARN  "+DEFA+__FILE__+":"+__func__+"(line:"+std::to_string(__LINE__)+") "+text+"\n";\
if(isfile) if(!WriteLog(data))\
std::cout<<__FTIME__+" WARN "+__FILE__+":"+__func__+":"+std::to_string(__LINE__)+" Log file write failed\n";\
if(log_output_level>1)std::cout<<data;\
}while(0)

/*INFO（通知）:此信息输出后，主要是记录系统运行状态等关联信息。*/
#define LOG_Info(text) do{\
std::string data = __FTIME__+GREEN+" INFO  "+DEFA+__FILE__+":"+__func__+"(line:"+std::to_string(__LINE__)+") "+text+"\n";\
if(isfile) if(!WriteLog(data))\
std::cout<<__FTIME__+" INFO "+__FILE__+":"+__func__+":"+std::to_string(__LINE__)+" Log file write failed\n";\
if(log_output_level>2)std::cout<<data;\
}while(0)

/*DEBUG（调试）:最细粒度的输出，除却上面各种情况后，你希望输出的相关信息，都可以在这里输出。*/
#define LOG_Debug(text) do{\
std::string data = __FTIME__+WHITE+" DEBUG "+DEFA+__FILE__+":"+__func__+"(line:"+std::to_string(__LINE__)+") "+text+"\n";\
if(isfile) if(!WriteLog(data))\
std::cout<<__FTIME__+" DEBUG "+__FILE__+":"+__func__+":"+std::to_string(__LINE__)+" Log file write failed\n";\
if(log_output_level>3)std::cout<<data;\
}while(0)

/*TRACE（跟踪）:最细粒度的输出，除却上面各种情况后，你希望输出的相关信息，都可以在这里输出。*/
#define LOG_Trace(text) do{\
std::string data = __FTIME__+WHITE+" TRACE "+DEFA+__FILE__+":"+__func__+"(line:"+std::to_string(__LINE__)+") "+text+"\n";\
if(isfile) if(!WriteLog(data))\
std::cout<<__FTIME__+" TRACE "+DEFA+__FILE__+":"+__func__+":"+std::to_string(__LINE__)+" Log file write failed\n";\
if(log_output_level>4)std::cout<<data;\
}while(0)

#endif
