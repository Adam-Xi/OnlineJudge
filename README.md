# 项目简介
在线判题系统的业务部分实现<br />
用户可以完成在浏览器中访问域名获取试题列表，选中试题进行编写代码，提交到服务器进行编译运行，并将结果返回给浏览器端等操作

# 环境及相关类库
+ CentOS7 
+ g++7.3.0
+ httplib
+ jsoncpp
+ ctemplate模板库,详情参考 [ctemplate-Gitee](https://gitee.com/HGtz2222/ThirdPartLibForCpp)
```bash
# 安装：在上述链接中下载这个模板类库，并进入 el7.x86_64 目录下，执行 install.sh 安装脚本
# 环境配置：在~/.bash_profile文件中，添加一行，如下: 
# vim ~/.bash_profile
# 注：添加如下一行代码，路径为绝对路径，具体路径视自身机器而定
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/.../OnlineJudge/src/ThirdPartLibForCpp/el7.x86_64/third_part/include
# source ~/.bash_profile  # 需要进行更新环境配置文件
``` 
<br />
关于项目设计思路及不足之处，参见CSDN博客 []()

<hr />
