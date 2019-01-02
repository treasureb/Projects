#ifndef _NO_LOCK_QUEUE_H_
#define _NO_LOCK_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <iostream>

using namespace std;

#ifdef max
#define max(x,y)({                   \
        typeof(x)_max1 = (x);        \
        typeof(y)_max2 = (y);        \
        (void)(&_max1 == &_max2);    \
        _max1 > _max2 ? _max1 : _max2;})
#endif  //end of max

#ifdef min
#define min(x,y)({                   \
        typeof(x)_min1 = (x);        \
        tyoeof(y)_min2 = (y);        \
        (void)(&_min1 == &min2);     \
        _min1 < _min2 ? _min1 : _min2;})
#endif //end of min



class Kfifo{
public:
    Kfifo(unsigned int size);
    ~Kfifo();

    unsigned int get(unsigned char *buffer,unsigned int len);
    unsigned int put(unsigned char *buffer,unsigned int len);
    static unsigned long roundup_power_of_two(unsigned long val);
private:
    inline bool is_power_of_2(unsigned long n){
        return (n != 0 && ((n & (n - 1)) == 0));
    }

    inline unsigned int unused(){
        return (mask_ + 1) - (in_ - out_);
    }

private:
    unsigned int size_;
    unsigned int in_;
    unsigned int out_;
    unsigned int mask_;
    unsigned char *buffer_;
};

struct proto{
    unsigned int msgid;
    unsigned int cmd;
    unsigned int info;

    proto()
    :msgid(0)
     ,cmd(0)
     ,info(0)
    {}
};

#endif
