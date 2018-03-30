#include<iostream>
using namespace std;

//抽象接口
class ReplaceAlgorithm
{
public:
    virtual void Replace() = 0;
    virtual ~ReplaceAlgorithm() {};

};
//三种具体的替换算法
class LRU_ReplaceAlgorithm:public ReplaceAlgorithm
{
public:
    void Replace()
    {
        cout<<"Least Recently Used replace alogrithm"<<endl;
    }
};

class FIFO_ReplaceAlgorithm:public ReplaceAlgorithm
{
public:
    void Replace()
    {
        cout<<"First in First out replace algorithm"<<endl;
    }
};

class Random_ReplaceAlgorithm:public ReplaceAlgorithm
{
public:
    void Replace()
    {
        cout<<"Random replace algorithm"<<endl;
    }
};

#if 0
//Cache需要用到替换算法
class Cache
{
public:
    Cache(ReplaceAlgorithm* ra)
    {
        _ra = ra;
    }
    
    ~Cache()
    {
        delete _ra;
    }

    void Replace()
    {
        _ra->Replace();
    }
private:
    ReplaceAlgorithm* _ra;
};

int main()
{
    //暴露了算法细节
    Cache cache(new LRU_ReplaceAlgorithm());
    cache.Replace();
    return 0;
}
#endif

#if 0
//Cache需要用到替换算法
enum RA{LRU,FIFO,RANDOM};//标签
class Cache
{
public:
    Cache(enum RA ra)
    {
        switch (ra){
            case LRU:
                _ra = new LRU_ReplaceAlgorithm();
                break;
            case FIFO:
                _ra = new FIFO_ReplaceAlgorithm();
                break;
            case RANDOM:
                _ra = new Random_ReplaceAlgorithm();
                break;
            default:
                _ra = nullptr;
                break;
        }
    }
    ~Cache()
    {
        delete _ra;
    }

    void Replace()
    {
        _ra->Replace();
    }
private:
    ReplaceAlgorithm* _ra;
};

int main()
{
    Cache cache(LRU);//指定标签即可
    cache.Replace();
    return 0;
}
#endif

//Cache需要用到替换算法
template<class RA>
class Cache
{
public:
    Cache(){}
    ~Cache(){}
    void Replace()
    {
        _ra->Replace();
    }
private:
    ReplaceAlgorithm* _ra;
};

int main()
{
    Cache<Random_ReplaceAlgorithm> cache;//模板实参
    cache.Replace();
    return 0;
}
