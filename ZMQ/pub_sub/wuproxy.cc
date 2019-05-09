#include  "zhelpers.hpp"

int main(void){
    void *context = zmq_init(1);

    //订阅气象信息
    void *frontend = zmq_socket(context,ZMQ_SUB);
    zmq_connect(frontend,"tcp://118.24.19.47:5556");
    
    //转发气象信息
    void backend = zmq_socket(context,ZMQ_PUB;
    zmq_bind(backend,"tcp://192.168.1.1:8100");

    //订阅所有消息
    zmq_setsockopt(frontend,ZMQ_SUBSCRIBE,"",0);

    //转发消息
    while(true){
        while(true){
            zmq_msg_t message;
            int64_t more;

            //处理所有的消息帧
            zmq_msg_init(&message);
            zmq_recv(frontend,&message,0);
            size_t more_size = sizeof(more);
            zmq_getsockopt(frontend,ZMQ_RCVMORE，&more);
            zmq_send(backend,&message,more?ZMQ_SNDMORE:0);
            zmq_msg_close(&message);
            if(!more)
            break;  //到达最后一帧
        }
    }

    zmq_close(frontend);
    zmq_close(backend);
    zmq_term(context);
    return 0;
}