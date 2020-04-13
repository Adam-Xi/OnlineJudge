#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

#include "toolsClass.hpp"

// 试题信息描述
typedef struct TestQuestionDescription
{
    std::string _id;
    std::string _name;
    std::string _path;  // 保存路径
    std::string _difficulty;  // 难易程度
}Question;

class OJ_Model
{
public:
    OJ_Model()
    {
        LoadAllQuestions("./config.cfg");  
    }

    // 加载 unordered_map 中的所有试题，用以展示在页面中 
    bool GetAllQuestions(std::vector<Question>* question)
    {
        for(const auto& kv : QuesMap_)
        {
            question->push_back(kv.second);
        }
        std::sort(question->begin(), question->end(), [](const Question& left, const Question& right) {
                      return std::stoi(left._id) < std::stoi(right._id);  // 在数组中按照升序进行排序
                  });
        return true;
    }

    // 加载指定 id 的试题
    bool GetOneQuestion(const std::string& id, std::string* description, std::string* header, Question* question)
    {
        // 根据 id 去查找对应题目的信息，即它的路径
        auto iter = QuesMap_.find(id);
        if(iter == QuesMap_.end())
        {
            LOG(ERROR, "Not Found Question id is ") << id << std::endl;
            return false;
        }
        *question = iter->second;

        // 加载具体单个题目信息，从保存的路径上去加载
        // description 描述信息
        int ret = FileTools::ReadDataFromFile(DescPath(iter->second._path), description);
        if(ret == -1)
        {
            LOG(ERROR, "Read description failed!") << std::endl;
            return false;
        }
        // header 头部信息
        ret = FileTools::ReadDataFromFile(HeaderPath(iter->second._path), header);
        if(ret == -1)
        {
            LOG(ERROR, "Read header failed!") << std::endl;
            return false;
        }
        return true;
    }

    // 拼接代码
    // 将用户提交的代码 和 本地测试代码合并成一份，等待后续写入并编译执行
    bool SplicingCode(std::string user_code, const std::string& ques_id, std::string& code)
    {
        code.clear();
        // 查找对应id的试题是否存在
        auto iter = QuesMap_.find(ques_id);
        if(iter == QuesMap_.end())
        {
            LOG(ERROR, "Cannot find question id is ") << ques_id << std::endl;
            return false;
        }

        std::string tail_code;
        int ret = FileTools::ReadDataFromFile(TailPath(iter->second._path), &tail_code);  // 获取测试代码
        if(ret < 0)
        {
            LOG(ERROR, "Open tail.cpp failed!");
            return false;
        }

        code = user_code + tail_code;  // 合并
        return true;
    }

private:
    // 将 oj_config.cfg 文件中的所有题目信息组织进 unordered_map 中 
    bool LoadAllQuestions(const std::string& ConfigPath)
    {
        std::ifstream input_file(ConfigPath.c_str());
        if(!input_file.is_open())
        {
            return false;
        }

        std::string line;
        while(std::getline(input_file, line))
        {
            // 1.切割字符串
            //     切割原型：题号 名称 路径 难度
            std::vector<std::string> info;
            StringTools::Split(line, " ", &info);
            if(4 != info.size())
            { 
                // 当前切分出错，但不能直接退出，转而加载其他题目信息   
                continue;
            }
            
            // 2.将切分后的内容保存到 unordered_map 中去
            Question ques;
            ques._id = info[0];
            ques._name = info[1];
            ques._path = info[2];
            ques._difficulty = info[3];
            QuesMap_[ques._id] = ques;
        }
        input_file.close();
        return true;
    }

    // 问题描述文件
    std::string DescPath(const std::string& ques_path)
    {
        return ques_path + "desc.txt";
    }
    // 头文件----代码框架文件
    std::string HeaderPath(const std::string& ques_path)
    {
        return ques_path + "header.cpp";
    }
    // 尾文件----代码测试文件
    std::string TailPath(const std::string& ques_path)
    {
        return ques_path + "tail.cpp";
    }

private:
    std::unordered_map<std::string, Question> QuesMap_;
};
