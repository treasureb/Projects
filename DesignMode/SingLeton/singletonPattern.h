#ifndef _SINGLETON__
#define _SINGLETON_
#include<mutex>

std::mutex thread_lock;
//hungry mode
class singleton1
{
private:
    singleton1()
    {}
    static singleton1 *instance;
public:
    static singleton1 *getInstance()
    {
        return instance;
    }

};
singleton1 *singleton1::instance = new singleton1();

//Lazy mode
class singleton2
{
private:
    singleton2()
    {}
    static singleton2 *instance;
public:
    static singleton2 *getInstance()
    {
        /*  The efficiency too low
         *   because add two judge
         */
        if(instance == nullptr){
            thread_lock.lock();
            if(instance == nullptr)
            {
                instance = new singleton2();
                return instance;
            }
            thread_lock.unlock();
        } 
        return nullptr;
    } 

};

singleton2 *singleton2::instance = nullptr;
#endif
