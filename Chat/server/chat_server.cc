#include"chat_server.h"

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
}
}

