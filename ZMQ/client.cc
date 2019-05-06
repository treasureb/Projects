#include <zmq.hpp>
#include <string>
#include <iostream>

int main(){
    
    zmq::context_t context(1);
    zmq::socket_t socket(context,ZMQ_REQ);
    
    socket.connect("tcp://localhost:8080");

    for(int request_nbr = 0;request_nbr != 10;++request_nbr){
        zmq::message_t request(5);
        memcpy(request.data(),"Hello",5);
        std::cout<<"Send Hello"<<request_nbr<<"..."<<std::endl;
        socket.send(request);

        /* Get the reply */
        zmq::message_t reply;
        socket.recv(&reply);
        std::cout<<"Receoved World"<<request_nbr<<std::endl;
    }

    return 0;
}
