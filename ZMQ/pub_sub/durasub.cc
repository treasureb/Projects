#include "../zhelpers.hpp"
#include <string>
#include <iostream>

int main(){
    
    zmq::context_t context(1);

    //connnct sub socket
    zmq::socket_t subscriber(context,ZMQ_SUB);
    subscriber.setsockopt(ZMQ_IDENTITY,"Hello",5);
    subscriver.setsockopt(ZMQ_SUBSCRIBE,"",0);
    subscriber.connect("tcp://localhost:5565");

    //send sync
    zmq::socket_t sync(context,ZMQ_PUSH);
    sync.connect("tcp://localhost:5564");
    s_send(sync,"");

    //get update and exit as signal
    while(true){
        std::string recv = s_recv(subscriber);
        std::cout<<recv<<std::endl;

        if(recv == "END"){
            break;
        }

    }
    zmq_close(subscirber);
    zmzmq_close(sync);
    q_term(context);

    return 0;
}