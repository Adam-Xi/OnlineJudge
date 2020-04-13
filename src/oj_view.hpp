#pragma once

// ctemplate 为Google提供的开源模板库，使用时需要配置环境变量，可以在 ~/.bash_profile 中加入 export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:...path_name/OnlineJudge，并运行即可
#include <ctemplate/template.h>
#include <vector>
#include <string>

#include "oj_model.hpp"

class OJ_View
{
public:
    // 渲染 HTML 页面，并将该页面返回给调用者
    static void RenderAllQuestionsHTML(std::string* html, std::vector<Question>& question)
    {
        // 1 获取数据字典-->将拿到的试题数据按照一定顺序保存在内存当中
        ctemplate::TemplateDictionary dict("all_questions");

        for(const auto& e : question)
        {
            ctemplate::TemplateDictionary* section_dict = dict.AddSectionDictionary("question");
            section_dict->SetValue("id", e._id);
            section_dict->SetValue("name", e._name);
            section_dict->SetValue("difficulty", e._difficulty);
        }

        // 2 获取模板类指针，加载预定义的 HTML 页面到内存当中去
        ctemplate::Template* temp_pointer = ctemplate::Template::GetTemplate("./WebPageTemplate/all_questions.html", ctemplate::DO_NOT_STRIP);

        // 3 渲染：用模板类的指针，将数据字典中的数据更新到 HTML 页面的内存中去
        temp_pointer->Expand(html, &dict);
    }

    static void RenderOneQuestion(const Question& question, std::string& description, std::string& header, std::string* html)
    {
        ctemplate::TemplateDictionary dict("question");
        dict.SetValue("id", question._id);
        dict.SetValue("name", question._name);
        dict.SetValue("difficulty", question._difficulty);
        dict.SetValue("description", description);
        dict.SetValue("header", header);

        ctemplate::Template* temp_pointer = ctemplate::Template::GetTemplate("./WebPageTemplate/question.html", ctemplate::DO_NOT_STRIP);
        temp_pointer->Expand(html, &dict);
    }

    static void RenderReason(const std::string& errorno, const std::string& reason, const std::string& stdout_reason, std::string* html)
    {
        ctemplate::TemplateDictionary dict("reason");
        dict.SetValue("errorno", errorno);
        dict.SetValue("reason", reason);
        dict.SetValue("stdout", stdout_reason);

        ctemplate::Template* tpl = ctemplate::Template::GetTemplate("./WebPageTemplate/reason.html", ctemplate::DO_NOT_STRIP);
        tpl->Expand(html, &dict);
    }
};
