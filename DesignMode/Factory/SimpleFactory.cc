#include<iostream>

using namespace std;

/*
 * 实现一个简单的计算器
 *
 */

class Operation
{
public:
//    Operation(double A,double B)
//    :_numberA(A)
//     ,_numberB(B)
//    {}

    virtual double GetResult()
    {
        double result = 0;
        return result;
    }

    void SetNumberA(double A)
    {
        _numberA = A;
    }

    void SetNumberB(double B)
    {
        _numberB = B;
    }

    double NumberA()
    {
        return _numberA;
    }

    double NumberB()
    {
        return _numberB;
    }

private:
    double _numberA;
    double _numberB;
};

class OperationAdd:public Operation
{
    virtual double GetResult()
    {
        double result = 0;
        result = NumberA() + NumberB(); 
        return result;
    }
};

class OperationSub:public Operation
{
    virtual double GetResult()
    {
        double result = 0;
        result = NumberA() - NumberB();
        return result;
    }
};

class OperationMul:public Operation
{
    virtual double GetResult()
    {
        double result = 0;
        result = NumberA()*NumberB();
            return result;
    }
};

class OperationDiv:public Operation
{
    virtual double GetResult()
    {
        double result = 0;
        if(NumberB() == 0)
            cout<<"除数不能为0";
        result = NumberA()/NumberB();
            return result;
    }
};

class OperationFactory
{
public:
    Operation* CreateOperate(char operate)
    {
        Operation* oper = nullptr;
        switch(operate)
        {
            case '+':{
                OperationAdd* add = new OperationAdd();
                oper = add;
                break;
                     }
            case '-':{
                OperationSub* sub = new OperationSub();
                oper = sub;
                break;
                    }
            case '*':{
                OperationMul* mul = new OperationMul();
                oper = mul;
                break;
                     }
            case '/':{
                OperationDiv* div = new OperationDiv();
                oper = div;
                break;
                     }               
        }

        return oper;
    }
};

int main()
{
    Operation op;
    OperationFactory* operfactory = new OperationFactory();
    Operation* oper = &op;
    oper = operfactory->CreateOperate('+');
    oper->SetNumberA(1);
    oper->SetNumberB(2);
    double result = oper->GetResult();
    cout<<result<<" "<<endl;
    return 0;
}
