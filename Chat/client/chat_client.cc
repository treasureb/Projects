#include "chat_client.h"
#include"../server/api.hpp"

namespace client{

//可以采用读取配置文件来读取ip和port
//例如使用json来组织配置格式
int ChatClient::Init(const std::string& server_ip,short server_port)
{
    _sock = socket(AF_INET,SOCK_DGRAM,0);
    if(_sock < 0){
        perror("socket");
        return -1;
    }
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    _server_addr.sin_port = htons(server_port);
    return 0;
}

int ChatClient::SetUserInfo(const std::string& name,const std::string& school)
{
    _name = name;
    _school = school;
    return 0;
}

void ChatClient::SendMsg(const std::string& msg)
{
    //需要加上客户端的姓名
    //客户端和服务器交互的接口要严格按照服务器提供的api来操作
    //即约定就是一个应用层协议
    
    //数据准备
    server::Data data;
    data.name = _name;
    data.school = _school;
    data.msg = msg;
    //约定了用户输入什么请求代表下线
    if(data.msg == "quit" || data.msg == "exit" || data.msg == "Q"){
        data.cmd = "quit";
    }
    std::string str;
    data.Serialize(&str);

    //真正发送数据,是按照json格式的内容
    sendto(_sock,str.c_str(),str.size(),0,(sockaddr*)&_server_addr,sizeof(_server_addr));
    return;
}

void ChatClient::RecvMsg(server::Data* data)
{
    char buf[1024 * 10] = {0};
    //由于对端的IP端口号即使服务器的IP端口号，所以没必要再获取
    ssize_t read_size = recvfrom(_sock,buf,sizeof(buf)-1,0,NULL,NULL);
    if(read_size < 0){
        perror("recvfrom");
        return;
    }
    buf[read_size] = '\0';
    data->UnSerialize(buf);
}

}// end client

//先实现一个简易版本的客户端
void* Send(void* arg)
{
    (void) arg;
    //循环发送数据
    client::ChatClient* client = reinterpret_cast<client::ChatClient*>(arg);
    while(true){
        std::string str;
        std::cin>>str;
        client->SendMsg(str);
    }
    return NULL;
}

void* Recv(void* arg)
{
    (void) arg;
    //循环接收数据
    client::ChatClient* client = reinterpret_cast<client::ChatClient*>(arg);
    while(true){
        server::Data data;
        client->RecvMsg(&data);
        std::cout<<"["<<data.name<<"|"<<data.school<<"]"<<data.msg<<"\n";
    }
    return NULL;
}

int main(int argc,char* argv[])
{
    if(argc != 3){
        printf("Usage ./client [ip] [port]\n");
    }
    client::ChatClient client;
    client.Init(argv[1],atoi(argv[2]));
    std::cout<<"输入用户名:";
    std::string name,school;
    std::cin>>name;
    std::cout<<"输入学校:";
    std::cin>>school;
    client.SetUserInfo(name,school);

    //不科学
  //  std::string str;
  //  std::cin>>str;
  //  client.SendMsg(str);
  //  server::Data data;
  //  client.RecvMsg(&data);
    pthread_t stid,rtid;
    pthread_create(&stid,NULL,Send,&client);
    pthread_create(&rtid,NULL,Recv,&client);
    pthread_detach(stid);
    pthread_detach(rtid);
    return 0;
}
