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
#include <math.h>
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
    vector<double> getQueryWordsWeight(vector<string>&);
private:
    int _N = 183;
    MyConf & _conf;
    TcpConnectionPtr _conn;
    string _queryWord; //等查询的单词
    vector<WebPage> _pageLib;
    unordered_map<int,pair<int,int>> _offsetLib;
    unordered_map<string,set<pair<int,double>>> _invertIndexTable;
    map<string,double> _queryWeights;
    set<string> _stopWordsLib;
};

}//end of namespace wd

#endif
