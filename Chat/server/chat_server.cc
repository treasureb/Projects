#include"chat_server.h"
#include"api.hpp"
#include<sstream>

namespace server{

int ChatServer::Start(const std::string& ip,short port)
{
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    int ret = bind(fd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret < 0){
        perror("bind");
        return -1;
    }

    pthread_t productor,consumer;
    pthread_create(&productor,NULL,Product,this);
    pthread_create(&consumer,NULL,Consum,this);
    pthread_join(productor,NULL);
    pthread_join(consumer,NULL);
    return fd;
}


void* ChatServer::Consum(void* arg)
{
    //生产者线程，读取socket中的数据，然后写入BlockQueue
    ChatServer* server = reinterpret_cast<ChatServer*>(arg);
    //读取一次数据，并且写入BlockQueue
    while(true){
        server->RecvMsg();
    }
    //如果一个客户端执行完成，就返回了，然后线程就退出了
    //pthread_cancel终止掉一个线程
    //pthread_exit
    return NULL;
}

void* ChatServer::Product(void* arg)
{
    ChatServer* server = reinterpret_cast<ChatServer*>(arg);
    while(true){
        server->BroadCast();
    }
    return NULL;
}

int ChatServer::RecvMsg()

{
    //1.从socket中读取数据
    char buf[1024 * 5] = {0};
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    printf("recvfrom Begin:\n");
    ssize_t read_size = recvfrom(_sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&peer,&len);
    printf("recvfrom End:\n");
    if(read_size < 0){
        perror("recvfrom");
        return -1;
    }
    buf[read_size] = '\0';
    //2.把数据插入到BlockQueue
    Context context;
    context.str = buf;
    context.addr = peer;
    _queue.PushBack(context); 
    return 0;
}

int ChatServer::BroadCast()
{
    //1.从队列中读取一个元素
    Context context;
    _queue.PopFront(&context);
    //2.对取出的字符串进行反序列化
    Data data;
    data.UnSerialize(context.str);
    //3.根据这个消息更新在线好友列表
    if(data.cmd == "quit"){
        // b)如果这个成员当前是一个下线的消息(common是一个特殊的值)，把这样的一个成员从好友列表中删除
        DelUser(context.addr,data.name);
    }else{
        // a)如果这个成员当前不再好友列表中，加入进来
        // 使用[]方法来操作在线好友列表
        AddUser(context.addr,data.name);
    }
    // 4.遍历在线好友列表，将数据依次发送给每个客户端
    // (由于发送消息的用户中也存在于好友列表中，从而达到发送者可以看到自己发送的消息，但是此种方法较蠢)
    for(auto item:_online_friend_list){
        SendMsg(context.str,item.second);
    }
    return 0;
}

void ChatServer::AddUser(struct sockaddr_in addr,const std::string& name)
{
    //key相当于对ip地址和用户名进行拼接
    //使用name和ip共同进行拼接，本质上因为仅仅使用ip可能会出席那重复ip的情况(NAT技术)
    std::stringstream ss;
    ss <<name<<":"<<addr.sin_addr.s_addr;
    //[]map unordered_map
    //1.如果不存在，就插入
    //2.如果存在，就修改
    //ValueType& operator[](const KeyType& key)
    //const ValueType& operator[](const KeyType& key)const
    //insert  返回值是一个键值对<迭代器，bool>
    _online_friend_list[ss.str()] = addr;
}


void ChatServer::DelUser(struct sockaddr_in addr,const std::string& name)
{
    std::stringstream ss;
    ss<<name<<":"<<addr.sin_addr.s_addr;
    _online_friend_list.erase(ss.str());

}

void ChatServer::SendMsg(const std::string&data,struct sockaddr_in addr)
{
    //数据通过sendto发送给客户端
    sendto(_sock,data.c_str(),data.size(),0,(struct sockaddr*)&addr,sizeof(addr));
}


}//end server

