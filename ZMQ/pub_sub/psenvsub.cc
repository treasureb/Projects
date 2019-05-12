#include <iostream>
#include <string>
#include "../zhelpers.hpp"

int main(){

    //prepare our context and subscriber
    zmq::context_t context(1);
    zmq::socket_t subscribre(context,ZMQ_SUB);
    subscriber.connect("tcp://localhost:5563");
    subscriber.setsocket(ZMQ_SUBSCRIBE,"B",1);

    while(true){

        //read envelope with address
        std::string address = s_recv(subscriber);

        //read envelope contents
        std::string contents = s_recv(subscriber);

        std::cout<<"[" << address<<"]" << contents<<std::endl;
    }

    return 0;
}