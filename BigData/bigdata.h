#ifndef __BIGDATA_H__
#define __BIGDATA_H__

#include<string>

typedef long long INT64;

class BigData{
public:
    friend std::ostream& operator<<(std::ostream& os,const BigData* bg);

    BigData();
    BigData(INT64 value = 0);
    BigData(const std::string& value);

    BigData operator+(const BigData& bg);
    BigData operator-(const BigData& bg);
    BigData operator*(const BigData& bg);
    BigData operator/(const BigData& bg);

private:
    bool IsINT64OverFlow();
    INT64 value_;   //存放正常的小数据
    std::string strData_;    //存放大数据  
};
#endif  // end of __BIGDATA_H__
