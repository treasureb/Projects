#include "no_lock_queue.h"

void *consumer(void *arg){
    printf("consumer.\n");
    Kfifo *fifo = (Kfifo*) arg;
    if(!fifo){
        return NULL;
    }

    for(;;){
        proto p;
        unsigned int len = fifo->get((unsigned char*)&p,sizeof(p));
        if(len > 0){
            cout<<"-----------------------------"<<endl;
            cout<<"consumer proto msg id:"<<p.msgid<<endl;
            cout<<"consumer proto msg cmd:"<<p.cmd<<endl;
            cout<<"consumer proto msg info"<<p.info<<endl;
            cout<<"-----------------------------"<<endl;
        }
    }
    return (void*)fifo;
}

void *producer(void *args){
    Kfifo *fifo = (Kfifo*) args;

    if(!fifo){
        return NULL;
    }

    unsigned int i = 0;
    for(;;){
        proto p;
        p.msgid = i++;
        p.cmd = 333;
        p.info = 4444;
        fifo->put((unsigned char*)&p,sizeof(p));
        cout<<"producer put msgid:"<<p.msgid<<endl;
    }
    return (void*)fifo;
}


int main(){
    Kfifo *fifo = new Kfifo(1024);
    pthread_t consumeid,producerid;
    pthread_create(&producerid,NULL,producer,(void*)fifo);
    pthread_create(&consumeid,NULL,consumer,(void*)fifo);

    printf("info.\n");
    pthread_join(consumeid,NULL);
    pthread_join(producerid,NULL);
}
