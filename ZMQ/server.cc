#include <iostream>
#include <string> 
#include <zmq.hpp>
#include <unistd.h>

int main(){


    /* 准备上下文信息和套接字 */
    zmq::context_t context(1);
    zmq::socket_t socket(context,ZMQ_REP);
    socket.bind("tcp://*:8080");

    while(true){
        zmq::message_t request;

        /* 等待客户端请求 */
        socket.recv(&request);
        std::cout<<"收到 Hello"<<std::endl;

        /* 业务处理逻辑 */
        sleep(1);

        /* 对客户端的请求做出回应 */
        zmq::message_t reply(5);
        memcpy((void *)reply.data(),"World",5);
        socket.send(reply);
    }

    return 0;
}
