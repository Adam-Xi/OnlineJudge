#ifndef CompileOnline
// 此处为了编写用例的时候有语法提示，但是实际线上编译的过程中这个操作是不生效的
#include "header.cpp"
#endif

void Test()
{
    if(Solution().function() == string("Hello World!"))
    {
        cout << "Congratulation! You have accessed all test-cases!" << endl;
    }
    else
    {
        cout << "Unfortunately to say that you haven't accessed the test-cases!" << endl;
    }
}

int main()
{
    Test();
    return 0;
}

