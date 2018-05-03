#pragma once
#include<string>
#include<jsoncpp/json/json.h>

namespace server{

//此处的Data相当于服务器给客户端提供的API
struct Data
{
    std::string name;
    std::string school;
    std::string msg;
    //只有两种取值，quit或者空
    std::string cmd;

    void Serialize(std::string* output){
       // char buf[1024] = {0};
       // sprintf(buf,"{name:%s,school:%s，msg:%s,cmd:%s}",name.c_str(),school.c_str(),msg.c_str(),cmd.c_str());
       // *output = buf;
        Json::Value value;
        value["name"] = name;
        value["school"] = school;
        value["msg"] = msg;
        value["cmd"] = cmd;
        Json::FastWriter writer;
        *output = writer.write(value);
        return;
    }

    void UnSerialize(const std::string& input){
        Json::Value value;
        Json::Reader reader;
        reader.parse(input,value);
        //为了代码的严谨，加上判断函数
        //否则会抛出异常
        if(value["name"].isString()){
            name = value["name"].asString();
        }
        school = value["school"].asString();
        msg = value["msg"].asString();
        cmd = value["cmd"].asString();
        return;
    }
};

}//end server
