#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if (defined (WIN32))
#include <zhelpers.hpp?
#endif

#define within(num) (int) ((float) num * random() / (RAND_MAX + 1.0))

int main(){
    zmq::context_t context(1);
    zmq::socket_t publisher(context,ZMQ_PUB;
    publisher.bind("tcp://*:8080);
    publisher.bind("tcp://weather.ipc");

    //Initialize random number generator
    srandom((unsigned) time(NULL));
    while(true){
        
    }
}