#ifndef _PROXY_
#define _PROXY_

#include<iostream>
#include<string>
using namespace std;

/*
 *  虚代理模式
 *  考虑下面的场景
 *  1.打开一个很大的图片很耗时
 *  2.要求代开文档的速度很快
 *
 *  使用代理来管理大图片，当真正需要展现
 *  大图片时，由代理去具体创建
 */

//图片类
class Image
{
public:
    Image(string name)
        :_imageName(name)
    {}

    virtual ~Image(){}
    virtual void Show(){}
protected:
        string _imageName;
};

//大图片类
class BigImage:public Image
{
public:
    BigImage(string name)
        :Image(name)
    {}
    
    ~BigImage(){}
    
    void Show()
    {
        cout<<"Show Big Image:"<<_imageName<<endl;
    }
    
};

//大图片的代理
class BigImageProxy:public Image
{
public:
    BigImageProxy(string name)
        :Image(name)
         ,_bigImage(0)
    {}

    ~BigImageProxy()
    {
        delete _bigImage;
    }

    void Show()
    {
        if(_bigImage == nullptr)
            _bigImage = new BigImage(_imageName);

        _bigImage->Show();
    }
private:
    BigImage* _bigImage;
};
#endif
