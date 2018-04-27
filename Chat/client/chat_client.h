#pragma once
#include<iostream>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<pthread.h>

#include"../server/api.hpp"
typedef struct sockaddr_in sockaddr_in;

namespace client{

class ChatClient
{
public:
    //初始化客户端,主要进行socket的初始化
    int Init(const std::string& ip,short port);
    //设置用户信息
    int SetUserInfo(const std::string& name,const std::string& school);
    //发送消息
    void SendMsg(const std::string& msg);
    //接收消息
    void RecvMsg(server::Data* data);
private:
    //通过Init函数初始化
    int _sock;
    sockaddr_in _server_addr;
    std::string _name;
    std::string _school;
};

} //end client
