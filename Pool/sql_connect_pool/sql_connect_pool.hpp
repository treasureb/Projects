#ifndef __CONNECT_POOL_H__
#define __CONNECT_POOL_H__

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include <../locker.hpp>

using namespace std;

class connection_pool{
public:
    MYSQL *GetConnection();
    bool ReleaseConnection(MYSQL* conn);
    void DestroyPool();

    static connection_pool* GetInstance(string url,string User,string PassWord,string DatabaseName,int port,unsigned int MaxConn);
    int GetFreeConn();

    ~connection_pool();
private:
    unsigned int MaxConn;
    unsigned int CurConn;
    unsigned int FreeConn;
private:
    pthread_mutex_t lock;
    list<MYSQL *> connList;
    connection_pool *conn;
    MYSQL *con;
    connection_pool(string url,string User,string PassWord,string DatabaseName,int port,unsigned int MaxConn);
    static connection_pool *connPool;
    sem reserve;
private:
    string url;
    string Port;
    string User;
    string PassWord;
    string DatabaseName;
};

#endif  //end of __CONNECT_POOL_H__
