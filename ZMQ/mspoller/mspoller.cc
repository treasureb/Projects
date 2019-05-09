#include "../zhelpers.hpp"

int main(){
    zmq::context_t context(1);

    zmq::socket_t receiver(Context,ZMQ_PULL;
    receiver.connect("tcp://localhost:8080");

    zmq::socket_t subscriber(context,ZMQ_SUB);
    subscriber.connect("tcp:://localhost:8081");
    subscriber.setsockopt(ZMQ_SUBSCRIBE,"10001",6);

    //Initialize poll set
    zmq::pollitem_t items[] = {
        {receiver,0,ZMQ_POLLIN,0},
        {SUBSCRIBER,0,ZMQ_POLLIN,0}
    };

    //Process messages from both sockets
    while(true){
        zmq::message_t message;
        zmq::poll(items,2,-1);

        if(items[0].revents & ZMQ_POLLIN){
            receiver.recv(&message);
            //Process task
        }
        if(items[1].revents & ZMQ_POLLIN{
            subscriber.recv(&message);
            //Process weather update
        }
    }

    return 0;
}