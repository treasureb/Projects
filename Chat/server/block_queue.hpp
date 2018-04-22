#pragma once
#include<vector>
#include<semaphore.h>

namespace server{
//类模板 正确的叫法   C++是一种元编程
//模板类
template<typename T>
class BlockQueue{
public:
    BlockQueue(size_t s = 1024)
    :_data(s)
    ,_head(0)
    ,_tail(0)
    ,_size(0)
    {
        sem_init(&_sem_data,0,0);
        sem_init(&_sem_blank,0,s);
    }
  
    ~BlockQueue()
    {
        sem_destroy(&_sem_data);
        sem_destroy(&_sem_blank);
    }
    
    void PushBack(const T& value)
    {
        sem_wait(&_sem_blank);
        _data[_tail++] = value;
        if(_tail >= _data.size()){
            _tail = 0;
        }
        ++_size;
        sem_post(&_sem_data);
    }

    void PopFront(T* value)
    {
        sem_wait(&_sem_data);
        *value = _data[_head++];
        if(_head >= _data.size()){
            _head = 0;
        }
        --_size;
        sem_post(&_sem_blank);
    }
private:
    std::vector<T> _data;
    sem_t _sem_data;
    sem_t _sem_blank;
    //由于是单生产者，单消费者模型，所以可以不加互斥锁
    //sem_t _sem_lock;
    size_t _head;
    size_t _tail;
    size_t _size;
};

} //end server
