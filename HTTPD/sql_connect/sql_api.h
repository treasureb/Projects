#ifndef _SQL_
#define _SQL_

#include<iostream>
#include<string>
#include<mysql.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

class sqlApi
{
public:
    sqlApi(const std::string& _h,\
           const int& _port,\
           const std::string&_u = "root",\
           const std::string& _p = "",\
           const std::string& _db = "maple");

    int insert(const std::string& _name,\
               const std::string& _sex,\
               const std::string& _age,\
               const std::string& _schol,\
               const std::string& _seat);
    int connect();
    int select();
    ~sqlApi();
private:
    MYSQL* conn;
    MYSQL_RES* res;
    int port;
    std::string host;
    std::string user;
    std::string passwd;
    std::string db;
};
#endif
