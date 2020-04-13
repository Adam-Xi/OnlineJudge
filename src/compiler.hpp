#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string>
#include <iostream>
#include <jsoncpp/json/json.h>

#include "toolsClass.hpp"

enum ErrorNo
{
    OK = 0,  // 编译运行没有错误
    COMPILE_ERROR,  // 编译错误
    RUN_ERROR,  // 运行错误
    PRAM_ERROR,  // 参数错误
    INTERNAL_ERROR  // 内存错误
};

class Compiler
{
public:
    static void CompileAndRun(Json::Value req, Json::Value* resp)
    {
        // 1. 判空
        // {"code":"xxx", "stdin":"xxxx"}
        if(req["code"].empty())
        {
            (*resp)["errerno"] = PRAM_ERROR;
            (*resp)["reason"] = "Pram error";
            LOG(ERROR, "Request Command is Empty!") << std::endl;
            return ;
        }

        // 2.将完整代码 (用户代码 + 测试代码) 写到文件中去，对文件名称进行约定：tmp_时间戳.cpp
        std::string code = req["code"].asString();
        std::string tmp_filename = WriteTmpFile(code);
        if(tmp_filename == "")
        {
            (*resp)["errorno"] = INTERNAL_ERROR;
            (*resp)["reason"] = "Create file failed";
            LOG(ERROR, "Write Source failed!") << std::endl;
            return ;
        }

        // 3.编译
        if(!Compile(tmp_filename))
        {
            (*resp)["errorno"] = COMPILE_ERROR;
            std::string reason;
            FileTools::ReadDataFromFile(ErrorPath(tmp_filename), &reason);
            (*resp)["reason"] = reason;
            LOG(ERROR, "Compile Error!") << std::endl;
            return ;
        }

        // 4.运行
        int ret = Run(tmp_filename);
        if(ret != 0)
        {
            (*resp)["errorno"] = RUN_ERROR;
            (*resp)["reason"] = "Program exit by ret " + std::to_string(ret);
            LOG(ERROR, "Run Error!") << std::endl;
            return ;
        }

        // 5.构造响应 
        (*resp)["errorno"] = OK;
        (*resp)["reason"] = "Compile and run is okay";

        std::string stdout_reason;
        FileTools::ReadDataFromFile(StdoutPath(tmp_filename), &stdout_reason);
        (*resp)["stdout"] = stdout_reason;

        std::string stderr_reason;
        FileTools::ReadDataFromFile(StderrPath(tmp_filename), &stderr_reason);
        (*resp)["stderr"] = stderr_reason;

        // Clean(tmp_filename);
        return ;
    }

private:
    static std::string WriteTmpFile(const std::string& code)
    {
        // 1.组织文件名称
        std::string tmp_filename = "/tmp_" + std::to_string(TimeTools::TimeStamp());
        // 写文件
        int ret = FileTools::WriteDataToFile(SrcPath(tmp_filename), code);
        if(ret < 0)
        {
            LOG(ERROR, "Write code to source failed!") << std::endl;
            return "";
        }
        return tmp_filename;
    }

    static std::string SrcPath(const std::string& filename)
    {
        return "./tmp_files" + filename + ".cpp"; 
    }
    static std::string ErrorPath(const std::string& filename)
    {
        return "./tmp_files" + filename + ".err"; 
    }
    static std::string ExePath(const std::string& filename)
    {
        return "./tmp_files" + filename + ".exe"; 
    }
    static std::string StdoutPath(const std::string& filename)
    {
        return "./tmp_files" + filename + ".stdout"; 
    }
    static std::string StderrPath(const std::string& filename)
    {
        return "./tmp_files" + filename + ".stderr"; 
    }

    static bool Compile(const std::string& filename)
    {
        // 1.构造编译命令 --> g++ src -o [exec] -std=c++11
        const int commandcount = 20;
        char buf[commandcount][50] = {{0}};
        char* command[commandcount] = {0};
        for(int i = 0; i < commandcount; i++)
        {
            command[i] = buf[i];
        }
        snprintf(command[0], 49, "%s", "g++");
        snprintf(command[1], 49, "%s", SrcPath(filename).c_str());
        snprintf(command[2], 49, "%s", "-o");
        snprintf(command[3], 49, "%s", ExePath(filename).c_str());
        snprintf(command[4], 49, "%s", "-std=c++11");
        snprintf(command[5], 49, "%s", "-D");
        snprintf(command[6], 49, "%s", "CompileOnline");
        command[7] = NULL;

        // 2.创建子进程
        //   父进程 --> 等待子进程退出
        //   子进程 --> 进程程序替换
        int pid = fork();
        if(pid < 0)
        {
            LOG(ERROR, "Create child process failed!") << std::endl;
            return false;
        }
        else if(pid == 0)
        {
            // child
            int fd = open(ErrorPath(filename).c_str(), O_CREAT | O_RDWR, 0664);
            if(fd < 0)
            {
                LOG(ERROR, "Open Compile errorfile failed:") << ErrorPath(filename) << std::endl;
                exit(1);
            }

            // 重定向
            dup2(fd, 2);
            // 程序替换
            execvp(command[0], command);
            perror("execvp");
            LOG(ERROR, "Execvp failed!") << std::endl;
            exit(0);
        }
        else
        {
            // father
            waitpid(pid, NULL, 0);
        }

        // 3.验证是否产生可执行程序
        // int stat(const char* filename, struct stat* buf); 通过filename获取文件信息，将信息保存在buf中，成功返回0，失败返回-1
        struct stat st;
        int ret = stat(ExePath(filename).c_str(), &st);
        if(ret < 0)
        {
            LOG(ERROR, "Compile error! Exe_filename is ") << ExePath(filename) << std::endl;
            return false;
        }
        return true;
    }

    static int Run(std::string& filename)
    {
        // 1.创建子进程
        //   父进程 --> 进程等待
        //   子进程 --> 进程替换为编译出来的可执行程序
        int pid = fork();
        if(pid < 0)
        {
            LOG(ERROR, "Exec pragma failed! Create chile process failed!") << std::endl;
            return -1;
        }
        else if(pid == 0)
        {
            // child
            // 对于子进程执行的限制
            //   时间限制----alarm()
            alarm(1);
            //   内存大小限制
            struct rlimit rl;
            rl.rlim_cur = 1024 * 30000;
            rl.rlim_max = RLIM_INFINITY;  // 无限制
            setrlimit(RLIMIT_AS, &rl);

            // 获取标准输出，重定向到文件
            int stdout_fd = open(StdoutPath(filename).c_str(), O_CREAT | O_RDWR, 0664);
            if(stdout_fd < 0)
            {
                LOG(ERROR, "Open stdout file failed:") << StdoutPath(filename) << std::endl;
                return -1;
            }
            dup2(stdout_fd, 1);
            // 获取标准错误，重定向到文件
            int stderr_fd = open(StderrPath(filename).c_str(), O_CREAT | O_RDWR, 0664);
            if(stdout_fd < 0)
            {
                LOG(ERROR, "Open stderr file failed:") << StderrPath(filename) << std::endl;
                return -1;
            }
            dup2(stderr_fd, 2);

            execl(ExePath(filename).c_str(), ExePath(filename).c_str(), NULL);
            exit(1);
        }
        else
        {
            //father
            int status = -1;
            waitpid(pid, &status, 0);
            // 将是否收到信号的信息返回给调用者，如果调用者判断是0，则正常运行完毕，否则表明子进程时收到某个信号异常结束的
            return status & 0x7f;
        }
    }

    static void Clean(std::string filename)
    {
        unlink(SrcPath(filename).c_str());
        unlink(ExePath(filename).c_str());
        unlink(ErrorPath(filename).c_str());
        unlink(StdoutPath(filename).c_str());
        unlink(StderrPath(filename).c_str());
    }
};
