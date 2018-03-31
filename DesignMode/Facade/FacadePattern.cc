#include<iostream>

using namespace std;

/*
 * 编译器例子：需要通过以下几步
 * 1.词法分析，语法分析
 * 2.中间代码生成
 * 3.机器码生成
 */

class Scanner
{
public:
    void Scan()
    {
        cout<<"词法分析"<<endl;
    }
};

class Parser
{
public:
    void Parse()
    {
        cout<<"词法分析"<<endl;
    }
};

class GenMidCode
{
public:
    void GenCode()
    {
        cout<<"产生中间代码"<<endl;
    }
};

class GenMachineCode
{
public:
    void GenCode()
    {
        cout<<"产生机器码"<<endl;
    }
};

//高层接口
class Compiler
{
public:
    void Run()
    {
        Scanner scanner;
        Parser parser;
        GenMidCode genMidCode;
        GenMachineCode genMacCode;
        scanner.Scan();
        parser.Parse();
        genMidCode.GenCode();
        genMacCode.GenCode();
    }
};


//客户端使用
int main()
{
    //用户不用关心编译的细节，只要能编译成功就好
    Compiler compiler;
    compiler.Run();
    return 0;
}
