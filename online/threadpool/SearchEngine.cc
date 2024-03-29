#include "../../include/SearchEngine.h"
namespace wd
{

SearchEngine::SearchEngine(const string& cfgFileName)
    :_conf(MyConf::getInstance(cfgFileName))
     ,_tcpserver(_conf->getConfigMap().find("ip")->second,stoi(_conf->getConfigMap().find("port")->second))
     ,_threadpool(4,10)
    {}

void SearchEngine::onConnection(const TcpConnectionPtr& conn)
{
    cout << conn->toString() << "has Connected!" << endl;
}

void SearchEngine::onMessage(const TcpConnectionPtr& conn)
{
    cout << "onMessage...." << endl;
    string msg = conn ->receive();
    if(msg.back() == '\n')
        msg.back() = 0;
    cout << ">> receive msg from client: " << msg << endl;
    WordQuery task(msg,conn,_conf);
    _threadpool.addTask(std::bind(&WordQuery::sendResponce,task));
}

void SearchEngine::onClose(const TcpConnectionPtr& conn)
{
    cout << "onClose....." << endl;
    cout << conn->toString() << "has closed!" << endl;
}

using namespace std::placeholders;
void SearchEngine::start()
{//using namespace std::placeholders;
    _threadpool.start();
    _tcpserver.setConnectionCallback(bind(&SearchEngine::onConnection,this,_1));  
    _tcpserver.setMessageCallback(bind(&SearchEngine::onMessage,this,_1));
    _tcpserver.setCloseCallback(bind(&SearchEngine::onClose,this,_1));
    _tcpserver.start();
}
}//end of namespace wd
