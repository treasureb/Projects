#ifndef _SMARTREF_
#define _SMARTREF_

#include<iostream>
#include<string>
using namespace std;

template<class T>
class auto_ptr
{
public:
    //防止隐式类型转换
    explicit auto_ptr(T* p = nullptr)
        :pointer(p)
    {}
    
    //release释放旧指针
    auto_ptr(auto_ptr<T>& rhs)
        :pointer(rhs.release())
    {}

    auto_ptr<T>& operator=(auto_ptr<T>& rhs)
    {
        if(this != &rhs)
            reset(rhs.release());

        return *this;
    }

    T& operator*()const
    {
        return *pointer;
    }

    T* operator->()const
    {
        return pointer;
    }

    T* get()const
    {
        return pointer;
    }

    T* release()
    {
        T* oldPointer = pointer;
        pointer = nullptr;
        return oldPointer;
    }

    void reset(T* p = nullptr)
    {
        if(pointer != p){
            delete pointer;
            pointer = p;
        }
    }
private:
    T* pointer;
};


template <typename T>
class smart_ptr
{
public:
    smart_ptr(T* p = nullptr)
        :pointer(p)
         ,count(new size_t(1))
    {}

    smart_ptr(const smart_ptr& rhs)
        :pointer(rhs.pointer)
         ,count(rhs.count)
    {
        ++*count;
    }

    ~smart_ptr()
    {
        decr_count();
    }

    smart_ptr& operator=(const smart_ptr& rhs)
    {
        //自身赋值不出错，计数先减1，再加1
        ++(rhs.count);
        decr_count();
        pointer = rhs.pointer;
        count = rhs.count;

        return *this;
    }

    T* operator->()
    {
        return pointer;
    }
    const T* operator->()const
    {
        return *pointer;
    }
    
    T& operator*()
    {
        return *pointer;
    }
    
    const T& operator*()const
    {
        return *pointer;
    }

    size_t get_refcount()
    {
        return *count;
    }
private:
    T* pointer;
    size_t* count;

    void decr_count()
    {
        if(--*count == 0)
        {
            delete pointer;
            delete count;
            pointer = nullptr;
            count = nullptr;
        }
    }
};
#endif
