#pragma once
#include"BlockQueue.hpp"
#include<string>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<unordered_map>

namespace server{

class ChatServer
{
public:
    int Start(const std::string& ip,short port);
    //接收消息
    int RecvMsg();
    //广播消息
    int BroadCast();
private:
    static void* Consum(void*);
    static void* Product(void*);

    //key 内容用户身份标识，ip+昵称
    //value ip+port(struct sockaddr_in)
    std:: unordered_map<std::string,sockaddr_in> _online_friend_list;
    //TODO 实现一个阻塞队列，作为交易场所
    BlockQueue<std::string> _queue;
    int _sock;//服务器进行绑定的文件描述符
};

}//end server
