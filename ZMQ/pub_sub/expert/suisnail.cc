#include "../../zhelpers.hpp"

#define MAX_ALLOWED_DELAY 1000

static void *subscriber(void *args){
    zmq::context_t context(1);
    zmq::socket_t subscriber(context,ZMQ_SUB);
    subscriber.connect("tcp://localhost:5556");

    std::stringstream ss;
    //Get and process message
    while(true){
        ss.clear();
        ss.str(s_recv(subscriber));
        int64_t clock = 0;
        assert((ss >> clock));

        //Suicide snail logic
        if(s_clock() - clock > MAX_ALLOWED_DELAY){
            std::cerr<<"E : subscriber cannot keep up,aborting"<<std::endl;
            break;
        }

        s_sleep(1000 * (1+within(2)));
    }

    return NULL;
}


static void *publiser(void *arg){
    zmq::context_t context(1);
    zmq::socket_t publiser(context,ZMQ_PUB);
    publiser.bind("tcp://5556");

    std::stringstream ss;

    while(true){
        //send current closk(msecs) to subscriber
        ss.str("");
        ss << s_clock();
        s_send(publiser,ss.str());

        s_sleep(1);
    }

    return 0;
}

int main(){
    pthread_t server_thread;
    pthread_create(&server_thread,NULL,publiser,NULL);

    pthread_t client_thread;
    pthread_create(&client_thread,NULL,subscriber,NULL);
    pthread_join(client_thread,NULL);

    return 0;
}
