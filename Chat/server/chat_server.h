#pragma once
#include"block_queue.hpp"
#include"api.hpp"
#include<string>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<unordered_map>

namespace server{

struct Context
{
    std::string str;
    struct sockaddr_in addr; 
};

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
    void AddUser(struct sockaddr_in addr,const std::string& name);
    void DelUser(struct sockaddr_in addr,const std::string& name);
    void SendMsg(const std::string& data,struct sockaddr_in addr);
    

    //key 内容用户身份标识，ip+昵称
    //value ip+port(struct sockaddr_in)
    std:: unordered_map<std::string,sockaddr_in> _online_friend_list;
    //TODO 实现一个阻塞队列，作为交易场所
    BlockQueue<Context> _queue;
    int _sock;//服务器进行绑定的文件描述符
};

}//end server
