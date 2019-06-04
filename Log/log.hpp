#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "block_queue.hpp"
using namespace std;


#define LOG_DEBUG(format, ...) Log::get_instance()->write_log(0,format,__VA_ARGS__)
#define LOG_INFO(format,...) Log::get_instance()->write_log(1,format,__VA_ARGS__)
#define LOG_WARN(format,...) Log::get_instance()->write_log(2,format,__VA_ARGS__)
#define LOG_ERROR(format,...) Log::get_instance()->write_log(3,format,__VA_ARGS__)


class Log{
public:
    static Log* get_instance(){
        static Log instance;
        return &instance;
    }

    static void *flush_log_thread(void *args){
        Log::get_instance()->async_write_log();
    }

    bool init(const char* file_name,int log_buf_size = 8192,int split_lines = 5000000,int max_queue_size = 0);
    void write_log(int level,const char* format,...);
    void flush(void);

private:
    Log();
    virtual ~Log();
    void *async_write_log(){
        string single_log;
        /* 从阻塞队列中取出一个日志，写入文件 */
        while(m_log_queue->pop(single_log)){
            pthread_mutex_lock(m_mutex);
            fputs(single_log.c_str(),m_fp);
            pthread_mutex_unlock(m_mutex);
        }
    }

private:
    pthread_mutex_t *m_mutex;       //互斥锁
    char dir_name[128];             //路径名
    char log_name[128];             //log文件名
    int m_split_lines;              //日志最大行数
    int m_log_buf_size;             //日志缓冲区大小
    long long m_count;              //日志行数记载
    int m_today;                    //记录当前日期
    FILE *m_fp;                     //打开log文件指针
    char *m_buf;                    
    block_queue<string> *m_log_queue;//阻塞队列
    bool m_is_async;                //同步标志
};

#endif  //end of __LOG_H__
