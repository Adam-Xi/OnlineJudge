#pragma once

#include <sys/time.h>
#include <assert.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

// 字符串工具类
class StringTools
{
public:
    // 切分字符串
    static void Split(const std::string& input, const std::string& split_char, std::vector<std::string>* output)
    {
        // boost::split(type, select_list, boost::is_any_of(","), boost::token_compress_on);
        // type 用于存放切割之后的字符串，传址，不一定非得是vector，换成其他vector容器也是可以的
        // select_list 传入的字符串（待切割的内容, string 类型），可以为空
        // boost::is_any_of(",") 设定切割条件符号为,(逗号)
        // boost::token_compress_on 将连续多个分隔符当做一个，默认为...off
        boost::split(*output, input, boost::is_any_of(split_char), boost::token_compress_off);
    }
};

// 时间工具类
class TimeTools
{
public:
    // 获取系统时间戳
    static int64_t TimeStamp()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec;
    }

    // 获取时间戳，并构造成常规认知的时间格式字符串
    // 年-月-日 时:分:秒
    static void TimeStampMS(std::string* TimeStamp)
    {
        time_t SysTime;
        time(&SysTime);
        struct tm* st = localtime(&SysTime);
        char buf[32] = {'\0'};
        snprintf(buf, sizeof(buf) - 1, "%04d-%02d-%02d %02d:%02d:%02d", st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);
        TimeStamp->assign(buf, strlen(buf));
    }
};

// 日志等级
const char* Level[] = { "INFO", "WARNING", "ERROR", "FATAL", "DEBUG" };

enum LogLevel
{
    INFO = 0,
    WARNING,
    ERROR,
    FATAL,
    DEBUG
};

inline std::ostream& Log(LogLevel lev, const char* file, int line, const std::string& logmsg)
{
    std::string level_info = Level[lev];
    std::string TimeStamp;
    TimeTools::TimeStampMS(&TimeStamp);

    // [时间 日志等级 文件:行号] 具体的日志信息
    std::cout << "[" << TimeStamp << " " << level_info << " " << file << ":" << line << "]" << " " << logmsg;
    return std::cout;
}
#define LOG(lev, msg) Log(lev, __FILE__, __LINE__, msg)


// 文件工具类
class FileTools
{
public:
    static int ReadDataFromFile(const std::string& filename, std::string* content)
    {
        std::ifstream file(filename.c_str());
        if(!file.is_open())
        {
            LOG(ERROR, "Open file failed! Filename is ") << filename << std::endl;
            return -1;
        }

        std::string line;
        while(std::getline(file, line))
        {
            *content += line + "\n";
        }
        file.close();
        return 0;
    }

    static int WriteDataToFile(const std::string& filename, const std::string& data)
    {
        std::ofstream file(filename.c_str());
        if(!file.is_open())
        {
            LOG(ERROR, "Open file failed! Filename is ") << filename << std::endl;
            return -1;
        }

        file.write(data.data(), data.size());
        file.close();
        return 0;
    }
};

// URL 编码操作类
class URLTools
{
public:
    static void PraseBody(const std::string& body, std::unordered_map<std::string, std::string>* pram)
    {
        //name=xxx&stdin=xxx
        std::vector<std::string> tokens;
        StringTools::Split(body, "&", &tokens);
        for(const auto& token : tokens)
        {
            // name=xxx
            // stdin=xxxx
            // ...
            std::vector<std::string> vec;
            StringTools::Split(token, "=", &vec);
            if(2 != vec.size())
            {
                continue;
            }
            (*pram)[vec[0]] = URLDecode(vec[1]);
        }
    }
    
private:
    static unsigned char ToHex(unsigned char c)
    {
        return c > 9 ? c + 55 : c + 48;
    }

    static unsigned char FromHex(unsigned char x)
    {
        unsigned char y;
        if(x >= 'A' && x <= 'Z')
            y = x - 'A' + 10;
        else if(x >= 'a' && x <= 'z')
            y = x - 'a' + 10;
        else if(x >= '0' && x <= '9')
            y = x - '0';
        else
            assert(0);
        return y;
    }
    static std::string URLEncode(const std::string& str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for(size_t i = 0; i < length; i++)
        {
            if(isalnum((unsigned char)str[i]) || 
               (str[i] == '-') || 
               (str[i] == '_') || 
               (str[i] == '.') || 
               (str[i] == '~'))
            {
                strTemp += str[i];
            }
            else if(str[i] == ' ')
            {
                strTemp += '+';
            }
            else
            {
                strTemp += '%';
                strTemp += ToHex((unsigned char)str[i] >> 4);
                strTemp += ToHex((unsigned char)str[i] % 16);
            }
        }
        return strTemp;
    }

    static std::string URLDecode(const std::string& str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for(size_t i = 0; i < length; i++)
        {
            if(str[i] == '+')
            {
                strTemp += ' ';
            }
            else if(str[i] == '%')
            {
                assert(i + 2 < length);
                unsigned char high = FromHex((unsigned char)str[++i]);
                unsigned char low = FromHex((unsigned char)str[++i]);
                strTemp += high * 16 + low;
            }
            else
            {
                strTemp += str[i];
            }
        }
        return strTemp;
    }
};
