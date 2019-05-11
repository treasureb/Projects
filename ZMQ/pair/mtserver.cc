#include "../zhelpers.hpp"
#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include <string>
#include <iostream>

static void* worker_routine(void *arg){
    zmq::context_t *context = static_cast<zmq::context_t*>(arg);

    zmq::socket_t socket(*context,ZMQ_REP);
    socket.connect("inproc://workers");

    //具体的处理细节
}

int main(){
    //Prepare our context and sockets
    zmq::context_t context(1);
    zmq::socket_t clients(context,ZMQ_ROUTER);
    clients.bind("tcp://*:5555");
    
    zmq::socket_t workers(context,ZMQ_DEALER);
    workers.bind("inproc://workers");

    for(int thread_nbr = 0;thread_nbr != 5;++thread_nbr){
        pthread_t worker;
        pthread_create(&worker,NULL,worker_routine,(void*)&context);
    }

    //connect work threads to client threads voa a queue
    zmq::proxy(clients,workers,NULL);
    return 0;
}