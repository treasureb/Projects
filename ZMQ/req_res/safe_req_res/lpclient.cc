#include "../../zhelpers.hpp"

#include <sstream>

#define REQUEST_TIMEOUT     2500
#define REQUEST_RETRIES     3

static zmq::socket_t *s_client_socket(zmq::context_t &context){
    std::cout<<"I: connecting to server..."<<std::endl;
    zmq::socket_t *client = new zmq::socket_t(context,ZMQ_REQ);
    client->connect("tcp://localhost:5555");

    //Configure socket to not wait at close time
    int linger = 0;
    client->setsockopt(ZMQ_LINGER,&linger,sizeof(linger));
    return client;
}

int main(){
    zmq::context_t context(1);
    zmq::socket_t* client = s_client_socket(context);

    int sequence = 0;   //用来和收到的reply进行比对
    int retries_left = REQUEST_RETRIES; //重试次数

    while(retries_left){
        std::stringstream request;
        request << ++sequence;
        s_send(*client,request.str());  //先发送序列号过去
        sleep(1);

        bool expect_reply = true;
        while(expect_reply){
            //poll socket for a reply with timeout
            zmq::pollitem_t items[] = {{(void*)*client,0,ZMQ_POLLIN,0}};
            zmq::poll(&items[0],1,REQUEST_TIMEOUT);

            //If we got a reply,process it
            if(items[0].revents & ZMQ_POLLIN){
                std::string reply = s_recv(*client);
                if(atoi(reply.c_str()) == sequence){
                    std::cout<<"I server replied OK ("<<reply<<")"<<std::endl;
                    retries_left = REQUEST_RETRIES; //发送成功后重置重试次数
                    expect_reply = false;   //收到回复，故不再进入循环
                }else{
                    std::cout<<"E: malformed reply from server:"<<reply<<std::endl;
                }
            }else if(--retries_left == 0){
                std::cout<<"E: server seems to be offline,abandoning"<<std::endl;
            }else{  //进行重新连接操作
                std::cout<<"W: no response from server,retrying..."<<std::endl;

                delete client;
                client = s_client_socket(context);

                s_send(*client,request.str());
            }
        }
    }
    delete client;
    return 0;
}
