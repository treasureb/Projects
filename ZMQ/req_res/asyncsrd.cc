#include <vector>
#include <thread>
#include <memory>
#include <functional>

#include <zmq.hpp>
#include "../zhelpers.hpp"


class client_task{
public:
    client_task()
        :ctx_(1),
        client_socket_(ctx_,ZMQ_DEALER)
    {}

    void start(){
        char identity[10] = {};
        sprintf(identity,"%04x-%04x",within(0x10000),within(0x10000));
        printf("%s\n",identity);

        client_socket_.setsockopt(ZMQ_IDENTITY,identity,strlen(identity));
        client_socket_.connect("tcp://localhost:5570");

        zmq::pollitem_t items[] = {
            {(void*)client_socket_,0,ZMQ_POLLIN,0}
        };
        int request_nbr = 0;
        try{
            while(true){
                for(int i = 0;i < 100;++i){
                    //10 milliseconds
                    zmq::poll(items,1,10);
                    if(items[0].revents & ZMQ_POLLIN){
                        printf("\n%s",identity);
                        s_dump(client_socket_);
                    }
                }
                char request_string[16] = {};
                sprintf(request_string,"request #%d",++request_nbr);
                client_socket_.send(request_string,strlen(request_string));
            }
        }
        catch(std::exception &e) {}
    }

private:
    zmq::context_t ctx_;
    zmq::socket_t client_socket_;
};

class server_worker{
public:
    server_worker(zmq::context_t &ctx,int sock_type)
        :ctx_(ctx)
         ,worker_(ctx_,sock_type)
    {}

    void work(){
        worker_.connect("inproc://backend");

        try{
            while(true){
                zmq::message_t identity;
                zmq::message_t msg;
                zmq::message_t copied_id;
                zmq::message_t copied_msg;
                worker_.recv(&identity);
                worker_.recv(&msg);

                int replies = within(5);
                for(int reply = 0;reply < replies;++reply){
                    s_sleep(within(1000)+1);
                    copied_id.copy(&identity);
                    copied_msg.copy(&msg);
                    worker_.send(copied_id,ZMQ_SNDMORE);
                    worker_.send(copied_msg);
                }
            }
        }
        catch(std::exception &e) {}
    }

private:
    zmq::context_t &ctx_;
    zmq::socket_t worker_;
};

class server_task{
public:
    server_task()
        :ctx_(1),
         frontend_(ctx_,ZMQ_ROUTER),
         backend_(ctx_,ZMQ_DEALER)
    {}

    enum{ kMaxTread = 5 };

    void run(){
         frontend_.bind("tcp://*:5570");
         backend_.bind("inproc://backend");

         std::vector<server_worker *> worker;
         std::vector<std::thread *> worker_thread;
         for(int i = 0;i < kMaxTread;++i){
             worker.push_back(new server_worker(ctx_,ZMQ_DEALER));

             worker_thread.push_back(new std::thread(std::bind(&server_worker::work,worker[i])));
             worker_thread[i]->detach();
         }

         try{
             zmq::proxy((void*)frontend_,(void*)backend_,nullptr);
         }

         catch(std::exception &e){}

         for(int i = 0;i < kMaxTread;++i){
             delete worker[i];
             delete worker_thread[i];
         }
    }

private:
    zmq::context_t ctx_;
    zmq::socket_t frontend_;
    zmq::socket_t backend_;
};

int main(){
    client_task ct1;
    client_task ct2;
    client_task ct3;
    server_task st;

    /* bind函数可以适配任意可调用对象, */
    /* 1.第一个参数为可调用对象 */
    /* 2.后面的参数个数为第一个对象的参数个数 */
    /* 3.可使用占位符(std::is_placeholder::_1) */

    /* thread第一个参数为线程处理函数，第二个参数为处理函数的参数 */
    std::thread t1(std::bind(&client_task::start,&ct1));
    std::thread t2(std::bind(&client_task::start,&ct2));
    std::thread t3(std::bind(&client_task::start,&ct3));
    std::thread t4(std::bind(&client_task::start,&st));

    t1.detach();
    t2.detach();
    t3.detach();
    t4.detach();

    getchar();

    return 0;

}
