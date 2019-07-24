#include "../../include/WordQuery.h"
#include "../../include/Thread.h"
#include "../../include/cppjieba/include/cppjieba/Jieba.hpp"

namespace wd
{

const char* const DICT_PATH = "../../include/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../../include/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../../include/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../../include/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../../include/cppjieba/dict/stop_words.utf8";

WordQuery::WordQuery(const string& queryWord,const TcpConnectionPtr& conn,MyConf& conf)
    :_conf(conf)
    ,_conn(conn)
    ,_queryWord(queryWord)
{}

void WordQuery::loadLibrary()
{
    ifstream ifs1,ifs2,ifs3;
    ifs2.open(_conf.getConfigMap().find("offsetPath")->second);
    int docId,offset,len;
    while(ifs2>>docId>>offset>>len)
    {
        _offsetLib.emplace(docId,offset,len); //未经测试
    }
    ifs2.close();

    ifs3.open(_conf.getConfigMap().find("indexPath")->second);
    string word;
    double weight;
    while(ifs3>>word>>docId>>weight)
    {
        _invertIndexTable.emplace(word,docId,weight);
    }
    ifs3.close();

    ifs1.open(_conf.getConfigMap().find("pagePath")->second);
    string txt,title,url,content;
    for(auto& off:_offsetLib)
    {
        offset = off.second.first;
        len = off.second.second;
        char buf[65535] = {0};
        ifs3.seekg(offset,ifs3.beg);
        ifs3.read(buf,len);
        txt = buf;
        int sbeg = txt.find("<title>")+7;
        int send = txt.find("</title>",sbeg);
        title = txt.substr(sbeg,send-sbeg);
        sbeg = txt.find("<url>")+5;
        send = txt.find("</url>",sbeg);
        url = txt.substr(sbeg,send-sbeg);
        sbeg = txt.find("<content>")+9;
        send = txt.find("</content>",sbeg);
        content = txt.substr(sbeg,send-sbeg);
        _pageLib.emplace_back(title,url,content);
    }
    ifs1.close();

    _stopWordsLib = _conf.getStopWordList();
}

string WordQuery::doQuery()
{
   cppjieba::Jieba jieba(DICT_PATH,
                         HMM_PATH,
                         USER_DICT_PATH,
                         IDF_PATH,
                         STOP_WORD_PATH);
   vector<string> querywords;
   jieba.Cut(_queryWord,querywords,true);
   vector<double> queryWordsWeight = getQueryWordsWeight(querywords);
   double sumWeights = 0;
   for(auto& w:queryWordsWeight)
   {
       sumWeights += w * w;
   }
   for(auto& q:_queryWeights)
   {
       q.second = q.second/sqrt(sumWeights);
   }
}

vector<double> WordQuery::getQueryWordsWeight(vector<string>& querywords)
{
   map<string,int> queryMap;
   set<string> words;
   vector<double> weights;
   double tf,df,idf,weight;
   for(auto& word:querywords)
   {
       ++queryMap[word];
       words.insert(word);
   }
   for(auto& w:words)
   {
       tf = queryMap[w];
       df = _invertIndexTable[w].size()+1;
       idf = log((_N+1)/df);
       weight = tf * idf;
       weights.push_back(weight);
       _queryWeights[w] = weight;
   }
   return weights;
}
}//end of namespace wd
