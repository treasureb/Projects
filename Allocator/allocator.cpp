#if 0
#include <iostream>
#define _THROW_BAD_ALLOC throw bad_alloc
#elif !defined(_THROW_BAD_ALLOC)
#include <iostream>
#define _THROW_BAD_ALLOC std::cerr <<"out of memory"<< std::endl;exit(1)
#endif

//一级空间配置器
template <int inst>
class _mallc_alloc_template
{
private:
    //以下函数用来处理内存不足的情况
    static void *Oom_malloc(size_t);
    static void *Oom_realloc(void*,size_t);
    //自定义的内存不足处理函数
    static void (* _Malloc_alloc_Oom_handler)();

public:

    static  void* Allocate(size_t n)
    {
        void *result = malloc(n); //第一级直接调用malloc向系统申请大块内存

        //当系统内存不足时，尝试调用oom_malloc()来释放内存
        if(0 == result)
        {
            result = Oom_malloc(n);
        }
        return result;
    }

    static void Deallocate(void *p,size_t)
    {
        free(p);//一级空间配置器直接调用free
    }

    static void* Reallocate(void *p,size_t ,size_t new_size)
    {
        void *result = realloc(p,new_size);//第一级直接调用realloc申请内存
        if(0 == result)
        {
            result = Oom_realloc(p,new_size);
        }
        return result;
    }

    //下面是仿真c++中的set_new_handler()
    static void(* set_malloc_handler(void (*f) ()))()
    {
        void (* old)() = _Malloc_alloc_Oom_handler;
        _Malloc_alloc_Oom_handler = f;
        return (old);
    }

};

template <int inst>
void (* _mallc_alloc_template<inst>:: _Malloc_alloc_Oom_handler)() = 0;

template<int inst>
void *_mallc_alloc_template<inst>::Oom_malloc(size_t n)
{
    void (* my_malloc_hander)();
    void * result;

    for(;;)//汇编中for的语句比while少
    {
        my_malloc_hander = _Malloc_alloc_Oom_handler;
        if(0 == my_malloc_hander)
        {
            _THROW_BAD_ALLOC;
        }
        (*my_malloc_hander)();//调用处理例程，企图释放内存
        result = malloc(n);//再次尝试申请内存

        if(result)//如果申请到内存就返回
        {
            return result;
        }
    }
}

template<int inst>
void *_mallc_alloc_template<inst>::Oom_realloc(void *p,size_t n)
{
    void (* my_malloc_hander)();
    void * result;

    for(;;)
    {
        my_malloc_hander = _Malloc_alloc_Oom_handler;
        if(0 == my_malloc_hander)
        {
            _THROW_BAD_ALLOC;
        }
        (*my_malloc_hander)();
        result = realloc(p,n);
        if(result)
        {
            return result;
        }
    }
}

typedef _mallc_alloc_template<0> malloc_alloc;


//二级空间配置器
/*
 * 二级空间配置器避免了产生了太多小额区块造成的外碎片，但是会产生内碎
 * 同时减少了系统频繁分配内存时的负担
 */
union obj
{
    union obj * free_list_link;
    char client_data[1];//不清楚作用
};

enum {_ALIGN = 8};//小型区块的上调边界
enum {_MAX_BYTES = 128};//小型区块的上限
enum {_NFREELISTS = _MAX_BYTES/_ALIGN};//free_list的个数

template<bool threads,int inst>//第一个模板参数为是否多线程，第二个模板参数留作备用
class _default_alloc_template
{
private:
    //ROUND_UP()将bytes上调置8的倍数，这样防止产生不必要的碎片问题
    static size_t ROUND_UP(size_t bytes)
    {
        return (((bytes)+_ALIGN-1) & (_ALIGN - 1));//后三位直接置0，则该数调整为8的整数倍
    }

private:
    union obj//free_list节点的构造
    {
        union obj* free_list_link;
        char client_data[1];
    };

private:
    //16个free_lists
    static obj *volatile free_list[_NFREELISTS];//利用哈希表的直接定址法
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return (((bytes) + _ALIGN-1) & _ALIGN - 1);//计算在数组中下标的位置
    }

    /*返回一个大小为n的对象，并可能加入大小为n的其他区块到freelist
     * 当自由链表中没有想对应大小的块时，去内存池中申请
    */
    static void *refill(size_t);
    
    //配置一大块空间可容纳nobjs个大小为size的区块
    //如果内存受限，nobjs的数量可能会有所下降
    static char *chunk_alloc(size_t size,int &nobjs);

    //Chunk allocation sate
    static char *start_free;//内存池的起始位置
    static char *end_free;//内存池的结束位置
    static size_t heap_size;//作为反馈参数

public:
    static void *allocate(size_t n);
    static void deallocate(void *p,size_t n);
    static void *reallocate(void *p,size_t old_size,size_t new_size);
};

template <bool threads,int inst>
char * _default_alloc_template<threads,inst>::start_free = 0;

template <bool threads,int inst>
char *_default_alloc_template<threads,inst>::end_free = 0;

template <bool threads,int inst>
size_t _default_alloc_template<threads,inst>::heap_size = 0;

template <bool threads,int inst>
//通过typename来告诉编译器，当obj *这个对象实例化化后再来初始化后面的对象，否则前面的对象都没实例化出对象，后面的对象无从实例化
typename _default_alloc_template<threads,inst>::obj * volatile 
_default_alloc_template<threads,inst>::free_list[_NFREELISTS] = 
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


/*
 * 空间配置函数
 *
 * 当申请的内存大于128bytes时，调用一级空间配置器
 * 当申请的内存小于128bytes时，去对应的自由链表中去获取节点，如果对应的自由链表出没有可用的节点，
 * 则将大小调整置8的倍数，然后调用refill() 去内存池中切去内存重新填充
 *
 */

template<bool threads,int inst>
void *_default_alloc_template<threads,inst>::allocate(size_t n)
{
    obj * volatile * my_free_list;
    obj * result;

    //如果大于128bytes就调用一级空间配置器
    if(n > (size_t) _MAX_BYTES)
    {
        return (malloc_alloc::Allocate(n));
    }

    my_free_list = free_list + FREELIST_INDEX(n);
    //如果对象的自由链表下有节点，则取头部的节点返回给调用者
    result = *my_free_list;

    //如果对应的自由链表出没有节点了，则调用refill重新填充
    if(0 == result)
    {
        void *r = refill(ROUND_UP(n));
        return r;
    }

    //调整free_list
    *my_free_list = result ->free_list_link;
    return result;
}


/*
 * 空间释放函数deallocate
 *
 * 大于128bytes的话，调用一级空间配置器
 * 小于128bytes，找出对用的free_list
 *
 */


template<bool threads,int inst>
void _default_alloc_template<threads,inst>::deallocate(void *p,size_t n)
{
    
    obj *q = (obj*)p;
    obj * volatile *my_free_list;

    //如果大于128bytes就调用一级空间配置器
    if(n > (size_t)_MAX_BYTES)
    {
        malloc_alloc::Deallocate(p,n);
        return;
    }
    //寻找对应的free_list
    my_free_list = free_list+FREELIST_INDEX(n);
    
    //将待回收的节点内存头插入自由链表
    q ->free_list_link = *my_free_list;
    *my_free_list = q;
}


/*
 * 重新填充free_list
 *
 *  当free_list中对应的自由链表中没有节点时，去调用refill()函数，重新填充。
 *  新的空间取自内存池(经由chunk_alloc())完成。缺省取得20个新节点，但万一内存池空间不足，获得的节点数可能小于20个
 *
 *  为什么一次要获取20块呢？
 *  考虑到多线程下，对于内存池中空间加锁解锁的开销
 */


//
template<bool threads,int inst>
void* _default_alloc_template<threads,inst>::refill(size_t n)
{
    int nobjs = 20;
    char *chunk = chunk_alloc(n,nobjs);
    obj * volatile *my_free_list;
    obj *result;
    obj *current_obj, *next_obj;
    int i;

    /* 如果只获得一个节点，则直接返回给调用者使用
     *  因为chunk_alloc函数会根据当前内存池的状态调整nobjs的个数
     */
    if(1 == nobjs)
    {
        return chunk;
    }
    //找到对应的自由链表，准备挂新的节点
    my_free_list = free_list+FREELIST_INDEX(n);
}
