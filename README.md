# 项目简介
在线判题系统业务实现

# 环境
+ CentOS7 
+ g++7.3.0
+ ctemplate模板库，代码内置，但需配置环境变量
> 方法：在~/.bash_profile文件中，添加一行，如下 
> #vim ~/.bash_profile
> # 注：添加如下一行代码，路径为绝对路径，具体路径视自身机器而定
> ```export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/.../OnlineJudge/src/ThirdPartLibForCpp/el7.x86_64/third_part/include```
> # source ~/.bash_profile  # 需要进行更新环境配置文件
