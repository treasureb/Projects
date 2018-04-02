#include<iostream> 
using namespace std;

class Builder
{
public:
    virtual void BuildHead(){}
    virtual void BuildBody(){}
    virtual void BuildLeftArm(){}
    virtual void BuildRightArm(){}
    virtual void BuildLeftLeg(){}
    virtual void BuildRightLeg(){}
};

//构造瘦人
class ThinBuilder:public Builder
{
public:
    void Builder()
    {
        cout<<"build thin body"<<endl;
    }
    void BuildBody()
    {
        cout<<"build thin head"<<endl;
    }
    void BuildLeftArm()
    {
        cout<<"build thin leftarm"<<endl;
    }
    void BuildRightArm()
    {
        cout<<"build thin rightarm"<<endl;
    }
    void BuildLeftLeg()
    {
        cout<<"build thin leftleg"<<endl;
    }
    void BuildRightLeg()
    {
        cout<<"build thin rightleg"<<endl;
    }
};

//构造胖人
class FatBuilder:public Builder
{
public:
    void Builder()
    {
        cout<<"build fat body"<<endl;
    }
    void BuildBody()
    {
        cout<<"build fat head"<<endl;
    }
    void BuildLeftArm()
    {
        cout<<"build fat leftarm"<<endl;
    }
    void BuildRightArm()
    {
        cout<<"build fat rightarm"<<endl;
    }
    void BuildLeftLeg()
    {
        cout<<"build fat leftleg"<<endl;
    }
    void BuildRightLeg()
    {
        cout<<"build fat rightleg"<<endl;
    }
};


//构造的指挥官
class Director
{
public:
    Director(Builder* builder)
    {
        _builder = builder;
    }

    void Create()
    {
        _builder->BuildHead();
        _builder->BuildBody();
        _builder->BuildLeftArm();
        _builder->BuildRightArm();
        _builder->BuildLeftLeg();
        _builder->BuildRightLeg();
    }
private:
    Builder* _builder;
};

//客户端代码
int main()
{
    ThinBuilder thin;
    Director director(&thin);
    director.Create();
    return 0;
}
