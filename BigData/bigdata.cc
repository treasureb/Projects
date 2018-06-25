#include"bigdata.h"
#include<string.h>
#include<iostream>


BigData::BigData(INT64 value):value_(value){

}

BigData::BigData(const std::string& strdata):value_(0),strData_("+0"){
    if(strData_.empty())
        return;

    char* pstr = (char*)strdata.c_str();

    while(isspace(*pstr))
        pstr++;
    if(*pstr == '\0')
        return;

    char symbol = pstr[0];
    if(symbol == '+' || symbol == '-')
        pstr++;
    else if(symbol >= '0' && symbol <= '9'){
        symbol = '+';
    }else{
        return;
    }

    //去掉前置0
    while(*pstr == '0')
        pstr++;

    if(*pstr == '\0')
        return;


    //尽量不要用push_back,扩容时效率太低
    strData_.resize(strlen(pstr)+1);
    strData_[0] = symbol;
    size_t count = 0;
    while(isdigit(*pstr)){
        strData_[count++] = *pstr;
        value_ = value_ * 10 + (*pstr - '0');
        ++pstr;
    }

    //value有可能为负值
    if(symbol == '-')
        value_ = -value_;

    //resize为有效字符的个数
    if(count > 1){
        strData_.resize(count);
    }
}


std::ostream& BigData::operator<<(std::ostream& _cout,const BigData& bg){
   const char* str = bg.BigData::strData_.c_str();
}

bool BigData:: IsINT64OverFlow(){
    std::string tmp("+9223372036854775807");
    if(tmp[0] == '-')
        tmp = "-9223372036854775807";

}
