#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <hiredis/hiredis.h>
using namespace std;

namespace wd
{
class Redis
{
public:
    Redis()
        :_connect(nullptr)
         ,_reply(nullptr)
    {}

    ~Redis()
    {
        if(_connect)
            redisFree(_connect);
        this->_connect = NULL;
        this->_reply = NULL;
    }

    bool connect(std::string host,int port)
    {
        this->_connect = redisConnect(host.c_str(),port);
        if(this->_connect != NULL && this->_connect->err)
        {
            cout << "connect error: " << this->_connect->errstr;
            return 0;
        }
        cout << "Connect to redisServer Success" << endl;
        return 1;
    }

    string get(string key)
    {
        this->_reply = (redisReply*) redisCommand(this->_connect,"GET %s",key.c_str());
        cout << "Success to execute command: get " << key.c_str() << endl;
        if(_reply->type == REDIS_REPLY_NIL)
            return string("-1");
        string str = this->_reply->str;
        freeReplyObject(this->_reply);
        return str;
    }

    void set(string key,string value)
    {
        redisCommand(this->_connect,"SET %s %s",key.c_str(),value.c_str());
        cout << "Succeed to execute command: set " << key.c_str() << " val" << endl;
    }
private:
    redisContext* _connect;
    redisReply* _reply;
};

}//end of namespace wd
