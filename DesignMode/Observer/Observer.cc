#include<iostream>
#include<string>
#include<list>
using namespace std;

//观察者
class Observer
{
public:
    Observer() {}
    virtual ~Observer(){}
    virtual void Update(){}
};

//博客类
class Blog
{
public:
    Blog(){}
    virtual ~Blog(){}
    void Attach(Observer* observer)
    {
        _observers.push_back(observer);
    }

    void Remove(Observer* observer)
    {
        _observers.remove(observer);
    }

    //通知观察者
    void Notify()
    {
        list<Observer*>::iterator it = _observers.begin();
        while(it != _observers.end())
            (*it)->Update();
    }

    //设置状态
    virtual void SetStatus(string s)
    {
        _status = s;
    }

    //获取状态
    virtual string GetStatus()
    {
        return _status;
    }
private:
    list<Observer*> _observers;
protected:
    string _status;
};

//具体的博客类
class BlogCSDN:public Blog
{
public:
    BlogCSDN(string name)
    :_name(name)
    {}

    ~BlogCSDN(){}
    void SetStatus(string s)
    {
        _status = "CSDN通知 : " + _name + s;
    }
    
    string GetStatus()
    {
        return _status;
    }
private:
    string _name;
};

//具体观察者
class ObserverBlog:public Observer
{
public:
    ObserverBlog(string name,Blog* blog)
    :_name(name)
    ,_blog(blog)
    {}
    ~ObserverBlog() {};
    void Update()//获得更新状态
    {
        string status = _blog->GetStatus();
        cout<<_name<<"--------"<<status<<endl;
    }
private:
    string _name;//观察者名称
    Blog* _blog;//观察的博客，如果以链表形式，则可以观察多个博客
};


//客户端
int main()
{
    Blog* blog = new BlogCSDN("maple");
    Observer* observer1 = new ObserverBlog("管理员",blog);
    blog->Attach(observer1);
    blog->SetStatus("发表观察者模式");
    blog->Notify();
    delete blog;
    delete observer1;
    return 0;
}
