#ifndef __WD_WordQuery_H_
#define __WD_WordQuery_H_
#include "TcpServer.h"
#include "MyConf.h"
#include "WebPage.h"
#include <queue>
#include <iostream>
#include <set>
#include <json/json.h>
#include <unordered_map>
using namespace std;

namespace wd
{
class WebPage;
class WordQuery
{
public:
    WordQuery(const string&,const wd::TcpConnectionPtr&,MyConf&);
    string doQuery(); //执行查询
    void loadLibrary(); //加载库文件
    vector<double> getQueryWordsWeight();
private:
    MyConf & _conf;
    TcpConnectionPtr _conn;
    string _queryWord; //等查询的单词
    vector<WebPage> _pageLib;
    unordered_map<int,pair<int,int>> _offsetLib;
    unordered_map<string,set<pair<int,double>>> _invertIndexTable;
};

}//end of namespace wd

#endif
