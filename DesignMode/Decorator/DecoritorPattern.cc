#include<iostream>
#include<string>
using namespace std;

//公共的抽象类
class Phone
{
public:
    Phone(){}
    virtual ~Phone(){}
    virtual void ShowDecorate(){}
};

//具体的手机类
class iPhone:public Phone
{
public:
    iPhone(string name)
    :_name(name)
    {}

    ~iPhone(){}
    void ShowDecorate()
    {
        cout<<_name<<"的装饰"<<endl;
    }
private:
        string _name;
};

//具体的手机类
class NokiaPhone:public Phone
{
public:
    NokiaPhone(string name)
    :_name(name)
    {}

    ~NokiaPhone(){}

    void ShowDecorate()
    {
        cout<<_name<<"的装饰"<<endl;
    }
private:
    string _name;
};

//装饰类
class DecoratorPhone:public Phone
{
public:
    DecoratorPhone(Phone* phone)
    :_phone(phone)
    {}

    virtual void ShowDecorate()
    {
        _phone->ShowDecorate();
    }
private:
    Phone* _phone;//具体要装饰的手机
};

//具体的装饰类
class DecoratorPhoneA:public DecoratorPhone
{
public:
    DecoratorPhoneA(Phone* phone)
    :DecoratorPhone(phone)
    {}

    void ShowDecorate()
    {
        DecoratorPhone::ShowDecorate();
        AddDecorate();
    }
private:
    void AddDecorate()
    {
        cout<<"增加挂件"<<endl;
    }
};

//具体的装饰类
class DecoratorPhoneB:public DecoratorPhone
{
public:
    DecoratorPhoneB(Phone* phone)
    :DecoratorPhone(phone)
    {}

    void ShowDecorate()
    {
        DecoratorPhone::ShowDecorate();
        AddDecorate();
    }
private:
    void AddDecorate()
    {
        cout<<"屏幕贴膜"<<endl;
    }
};

int main()
{
    Phone* iphone = new NokiaPhone("6300");
    Phone* dpa = new DecoratorPhoneA(iphone);
    Phone* dpb = new DecoratorPhoneB(iphone);
    dpb->ShowDecorate();
    delete dpa;
    delete dpb;
    delete iphone;
    return 0;
}
