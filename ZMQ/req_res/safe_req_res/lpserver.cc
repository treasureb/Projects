/* -直接输出请求内容 */
/* -随机地降慢运行速度，或终止程序，模拟崩溃 */

#include "../../zhelpers.hpp"


int main(void){
    srandom((unsigned)time(NULL));
    zmq::context_t context(1);
    zmq::socket_t server(context,ZMQ_REP);
    server.bind("tcp://*:5555");

    int cycles = 0;
    while(true){
        std::string request = s_recv(server);
        cycles++;

        if(cycles > 3 && within(3) == 0){
            std::cout<<"I : simulating a crash"<<std::endl;
            break;
        }else if(cycles > 3 && within(3) == 0){
            std::cout<<"I: normal request("<<request<<")"<<std::endl;
            sleep(1);
            s_send(server,request);
        }
    }       
    return 0;
}
