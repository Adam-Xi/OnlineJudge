 #include <iostream>
#include <vector>
#include <string>
using namespace std;

class Solution {
public:
    int Fibonacci(int n) 
    {

    }
};
#ifndef CompileOnline
// 此处为了编写用例的时候有语法提示，但是实际线上编译的过程中这个操作是不生效的
#include "header.cpp"
#endif

////////////////////////////////////// 
// 此处约定
// 1.每个用例是一个函数
// 2.每个用例从标准输出输出一行日志
// 3.吐过用例通过，同一打印[TestName] OK!
// 4.如果用例不通过，同一打印[TestName] Failed!并且给出错误提示
//////////////////////////////////////

void Test1()
{
    int ret = Solution().Fibonacci(1);
    if(ret == 1)
    {
        std::cout << "Test1 OK!" << std::endl;
    }
    else
    {
        std::cout << "Test1 failed! input:1, output:" << ret << std::endl;
    }
}

void Test2()
{
    int ret = Solution().Fibonacci(5);
    if(ret == 5)
    {
        std::cout << "Test2 OK" << std::endl;
    }
    else
    {
        std::cout << "Test2 failed! input:5, output:" << ret << std::endl;
    }
}

void Test3()
{
    int ret = Solution().Fibonacci(4);
    if(ret == 3)
    {
        std::cout << "Test3 OK" << std::endl;
    }
    else
    {
        std::cout << "Test3 failed! input:4, output:" << ret << std::endl;
    }
}

void Test4()
{
    int ret = Solution().Fibonacci(8);
    if(ret == 21)
    {
        std::cout << "Test4 OK" << std::endl;
    }
    else
    {
        std::cout << "Test4 failed! input:8, output:" << ret << std::endl;
    }
}

int main()
{
    Test1();
    Test2();
    Test3();
    Test4();
    return 0;
}
