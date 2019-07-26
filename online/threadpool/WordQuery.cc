#include "../../include/WordQuery.h"
#include "../../include/Thread.h"
#include "../../include/cppjieba/include/cppjieba/Jieba.hpp"

namespace wd
{
#if 0
const char* const DICT_PATH = "../../include/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../../include/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../../include/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../../include/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../../include/cppjieba/dict/stop_words.utf8";

struct MyCompare
{
    MyCompare(const map<string,double> weights)
        :_weights(weights)
    {}
    bool operator()(const pair<int,map<string,double>> &lhs,const pair<int,map<string,double>> &rhs)
    {
        map<string,double> lhsWeight = lhs.second;
        map<string,double> rhsWeight = rhs.second;
        double XY=0, XZ=0, sumX=0, sumY=0, sumZ=0;
        for(auto& it:_weights)
        {
            XY += it.second * lhsWeight[it.first];
            XZ += it.second * rhsWeight[it.first];
            sumX += pow(it.second,2);
            sumY += pow(lhsWeight[it.first],2);
            sumZ += pow(rhsWeight[it.first],2);
        }
        double cosXY = XY/(sqrt(sumX)*sqrt(sumY));
        double cosXZ = XZ/(sqrt(sumX)*sqrt(sumZ));
        if(cosXY < cosXZ)
            return false;
        else
            return true;
    }
    map<string,double> _weights;
};
#endif
WordQuery::WordQuery(const string& queryWord,const TcpConnectionPtr& conn,MyConf* conf)
    :_conf(conf)
    ,_conn(conn)
     ,_queryWord(queryWord)
{
    cout << "create finished." << endl;
}
#if 0
void WordQuery::loadLibrary()
{
    ifstream ifs1,ifs2,ifs3;
    ifs2.open(_conf->getConfigMap().find("offsetPath")->second);
    int docId,offset,len;
    while(ifs2>>docId>>offset>>len)
    {
        _offsetLib.emplace(docId,offset,len); //未经测试
    }
    ifs2.close();

    ifs3.open(_conf->getConfigMap().find("indexPath")->second);
    string word;
    double weight;
    while(ifs3>>word>>docId>>weight)
    {
        _invertIndexTable.emplace(word,docId,weight);
    }
    ifs3.close();

    ifs1.open(_conf->getConfigMap().find("pagePath")->second);
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

    _stopWordsLib = _conf->getStopWordList();
}
#endif
string WordQuery::doQuery()
{
#if 0
    loadLibrary();
    cppjieba::Jieba jieba(DICT_PATH,
                          HMM_PATH,
                          USER_DICT_PATH,
                          IDF_PATH,
                          STOP_WORD_PATH);
    vector<string> querywordsAll,querywords;
    jieba.Cut(_queryWord,querywordsAll,true);
    for(auto& a:querywordsAll)
    {
        auto it = _stopWordsLib.find(a);
        if(it == _stopWordsLib.end())
            querywords.push_back(a);
    }
    if(querywords.size() == 0)
        return returnNoAnswer();
    vector<double> queryWordsWeight = getQueryWordsWeight(querywords);
    double sumWeights = 0;
    for(auto& w:queryWordsWeight)
    {
        sumWeights += w * w;
    }
    vector<string> uniqueQueryWords;
    for(auto& q:_queryWeights)
    {
        q.second = q.second/sqrt(sumWeights);
        uniqueQueryWords.push_back(q.first);
    }
    vector<int> docIds = getDocIds(uniqueQueryWords);
    if(docIds.size() == 0)
        return returnNoAnswer();
    vector<pair<int,map<string,double>>> resultVector;
    for(auto& d:docIds)
    {
        for(auto& w:uniqueQueryWords)
        {
            set<pair<int,double>> tmp= _invertIndexTable[w];
            double weight;
            for(auto& p:tmp)
            {
                if(p.first == d)
                    weight = p.second;
            }
            resultVector.emplace_back(d,w,weight);
        }
    }
    MyCompare mcp(_queryWeights);
    stable_sort(resultVector.begin(),resultVector.end(),mcp);
    vector<int> resultIds;
    for(auto& re:resultVector)
    {
        resultIds.push_back(re.first);
    }
    string response = createJson(resultIds,uniqueQueryWords);
    return response;
#endif
    string str = "hi,baby,just test.";
    return str;
}

void WordQuery::sendResponce()
{
    cout << "enter WordQuery::sendResponce" << endl;
    string response = doQuery();
    cout << "response: " << response << endl;
    _conn->sendInLoop(response);
}
#if 0
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

vector<int> WordQuery::getDocIds(const vector<string>& words)
{
    vector<int> result;
    for(auto& i:_invertIndexTable[words[0]])
    {
        result.push_back(i.first);
    }
    for(auto it = words.begin()+1; it != words.end(); ++it)
    {
        vector<int> tmp,rhs;
        for(auto& s:_invertIndexTable[*it])
        {
            rhs.push_back(s.first);
        }
        set_intersection(result.begin(),result.end(),
                         rhs.begin(),rhs.end(),back_inserter(tmp));
        result.swap(tmp);
    }
    return result;
}

string WordQuery::createJson(vector<int>& docIds,vector<string>& querywords)
{
    Json::Value result;
    int maxCnt = 0;
    for(auto id:docIds)
    {
        if(maxCnt > 10)
            break;
        Json::Value tmp;
        tmp["title"] = _pageLib[id-1].getTitle();
        tmp["summary"] = _pageLib[id-1].getSummary(querywords);
        tmp["url"] = _pageLib[id-1].getUrl();
        tmp["content"] = _pageLib[id-1].getContent();
        result["webpage"].append(tmp);
        ++maxCnt;
    }
    Json::FastWriter fast_writer;
    string response = fast_writer.write(result);
    return response;
}

string WordQuery::returnNoAnswer()
{
    Json::Value result,tmp;
    tmp["title"] = "404. Not found.";
    tmp["summary"] = "Sorry,I cann't what you want.Please search something else.";
    tmp["url"] = "";
    tmp["content"] = "";
    result["webpage"].append(tmp);
    Json::FastWriter fast_writer;
    string response = fast_writer.write(result);
    return response;
}
#endif
}//end of namespace wd
