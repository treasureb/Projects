#include "no_lock_queue.h"

Kfifo::~Kfifo(){
    if(buffer_){
        free(buffer_);
        size_ = in_ = out_ = 0;
    }
}

/* 向上调整为2的整数倍 */
unsigned long Kfifo::roundup_power_of_two(unsigned long val){
    if((val & (val - 1)) == 0){
        return val;
    }

    unsigned long maxulong = (unsigned long)((unsigned long) ~0);
    unsigned long andv = ~(maxulong & (maxulong >> 1));
    while((andv & val) == 0){
        andv = andv >> 1;
    }

    return andv << 1;
}

Kfifo::Kfifo(unsigned int size)
    :size_(size)
     ,in_(0)
     ,out_(0)
     ,mask_(size - 1){

    if(!is_power_of_2(size)){
        size_ = roundup_power_of_two(size);
    }
    buffer_ = (unsigned char*)malloc(size);
}

unsigned int Kfifo::get(unsigned char *buffer,unsigned int len){
    unsigned int l;
    len = min(len,in_ - out_);
    __sync_synchronize();//使用内存屏障,来保障执行顺序

    l = min(len,size_ - (out_ & (size_ - 1)));
    memcpy(buffer,buffer_ + (out_ & (size_ - 1)),l);
    memcpy(buffer + l,buffer_,len - l);

    __sync_synchronize();

    out_ += len;
    return len;
}

unsigned int Kfifo::put(unsigned char *buffer,unsigned int len){
    unsigned int l;
    len = min(len,size_ - in_ + out_);
    __sync_synchronize();
    l = min(len,size_ - (in_ & (size_ - 1)));
    memcpy(buffer_ + (in_ & (size_ - 1)),buffer,l);
    memcpy(buffer_,buffer + l,len - l);
    __sync_synchronize();
    in_ += len;

    return len;
}
