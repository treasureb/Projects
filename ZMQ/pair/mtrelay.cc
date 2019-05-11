#include "../zhelpers.hpp"


void *stap1(void *arg){
    zmq::context_t* context = static_cast<zmq::context_t*>(arg);

    //signal downstream to step2
    zmq::socket_t sender(*context,ZMQ_PAIR);
    sender.connect("inproc://step2");

    s_send(sender,"");

    return (NULL);
}

void *step2(void *arg){
    zmq::context_t *context = static_cast<zmq::context_t*>(arg);

    zmq::socket_t receiver(*context,ZMQ_PAIR);
    receiver.bind("inproc://step2");

    pthread_t thread;
    pthreda_create(&thread,NULL,stap1,context);

    //wait for signal
    s_recv(receiver);

    //signal downstream to step3
    zmq::socket_t sender(*context,ZMQ_PAIR);
    sender.connect("inproc://step3");
    s_send(sender,"");

    return (NULL);
}

int main(){
    zmq::context_t context(1);

    //bind to inproc:endpoint,then start upstream thread
    zmq::socket_t receiver(context,ZMQ_PAIR);
    receiver.bind("inproc://step3");

    pthread_t thread;
    pthread_create(&thread,NULL,step2,&context);

    //wait for signal
    s_recv(receriver);

    std::cout<<"Test successful!"<<std::endl;

    return 0;
}
