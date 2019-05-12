#include <unisted.h>
#include "../zhelpers.hpp"

int main(){
    
    //创建上下文信息
    zmq::context_t context(1);
    zmq::socket_t publisher(context,ZMQ_PUB);
    publisher.bind("tcp://*:5563");

    while(true){

        //write two messae=ge,each whih an envelope and content
        s_sendmore(publisher,"A");
        s_send(publisher,"We don't want to see this");

        s_sendmore(publisher,"B");
        s_send(publisher,"We would like to see this");
        sleep(1);
    }
    
    return 0;
}
