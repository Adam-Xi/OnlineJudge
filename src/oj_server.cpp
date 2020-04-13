#include "httplib.h"
#include "oj_model.hpp"
#include "oj_view.hpp"
#include "compiler.hpp"

int main()
{
    using namespace httplib;
    Server server;
    OJ_Model oj_model;
    server.Get("/all_questions", [&oj_model](const Request& req, Response& resp) {
               (void)req;
               std::vector<Question> ques;
               oj_model.GetAllQuestions(&ques);

               // char buf[1024] = {'\0'};
               // printf("%lu\n", ques.size());
               // if(ques.size() == 1)
               // {
               //     snprintf(buf, sizeof(buf) - 1, "<html>%s.%s %s</html>", ques[0]._id.c_str(), ques[0]._name.c_str(), ques[0]._difficulty.c_str());
               // }
               // std::string html;
               // html.assign(buf, strlen(buf));
               // 使用模板技术填充 html 页面，用以替代上述固定方法填充, 如下：

               std::string html;
               OJ_View::RenderAllQuestionsHTML(&html, ques);
               // LOG(INFO, html);
               resp.set_content(html, "text/html; charset=UTF-8");
               });

    // 正则表达式:\b:单词的分界  *:匹配任意字符串  \d:匹配一个数字  ():分组应用
    // 源码转义:特殊字符就按照字符字面源码来编译  形如：R"(string)"
    server.Get(R"(/question/(\d+))", [&oj_model](const Request& req, Response& resp) {
               // question/1
               // 去试题模块中查找对应题号的具体的题目信息
               std::string desc;
               std::string header;
               
               // 从 query_str 中获取id
               LOG(INFO, "req.matches") << req.matches[0] << ":" << req.matches[1] << std::endl;
               // 在题目地址的路径中去加载单个题目的描述信息
               Question ques;
               oj_model.GetOneQuestion(req.matches[1].str(), &desc, &header, &ques);
               // 进行组织，返回给浏览器
               std::string html;
               OJ_View::RenderOneQuestion(ques, desc, header, &html);
               resp.set_content(html, "text/html; charset=UTF-8");
               });

    server.Post(R"(/question/(\d+))", [&oj_model](const Request& req, Response&  resp) {
                // 1.从正文中提取出来待提交的内容，主要是提取 code 字段所对应的内容
                //     提交的内容中有url编码 ---> 对提交的内容进行解码    ,并用 unordered_map 进行保存
                std::unordered_map<std::string, std::string> pram;
                URLTools::PraseBody(req.body, &pram);
                // for(const auto& e : pram)
                // {
                //     LOG(INFO, "code:") << e.second << std::endl;
                // }

                // 2.拼接代码：用户代码 + 测试代码
                std::string code;
                oj_model.SplicingCode(pram["code"], req.matches[1].str(), code);
                // LOG(INFO, "code: ") << code << std::endl;  // 查看拼接后的完整代码

                // 3.编译 && 运行
                Json::Value req_json;
                req_json["code"] = code;
                Json::Value resp_json;
                Compiler::CompileAndRun(req_json, &resp_json);

                // 4.构造响应
                const std::string errorno = resp_json["errorno"].asString();
                const std::string reason = resp_json["reason"].asString();
                const std::string stdout_reason = resp_json["stdout"].asString();
                std::string html;
                OJ_View::RenderReason(errorno, reason, stdout_reason, &html);
                resp.set_content(html, "text/html; charset=UTF-8");
                });
    LOG(INFO, "Listen in 0.0.0.0:9094") << std::endl;
    LOG(INFO, "Server ready!") << std::endl;
    server.listen("0.0.0.0", 9094);

    return 0;
}

