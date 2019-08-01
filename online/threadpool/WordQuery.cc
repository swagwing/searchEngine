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

WordQuery::WordQuery(const string& queryWord,const TcpConnectionPtr& conn,MyConf* conf)
    :_conf(conf)
    ,_conn(conn)
     ,_queryWord(queryWord)
{
    cout << "create finished." << endl;
}

void WordQuery::loadLibrary()
{
    ifstream ifs1,ifs2,ifs3;
    ifs2.open(_conf->getConfigMap().find("offsetPath")->second);
    int docId,offset,len;
    while(ifs2>>docId>>offset>>len)
    {
        _offsetLib.emplace(docId,make_pair(offset,len)); //未经测试
    }
    ifs2.close();

    ifs3.open(_conf->getConfigMap().find("indexPath")->second);
    string line,word;
    double weight;
    while(getline(ifs3,line))
    {
        stringstream ss(line);
        ss >> word;
        while(ss>>docId>>weight)
        {
            _invertIndexTable[word].insert(make_pair(docId,weight));
        }
    }
    ifs3.close();

    ifs1.open(_conf->getConfigMap().find("pagePath")->second);
    string txt,title,url,content;
    for(auto& off:_offsetLib)
    {
        offset = off.second.first;
        len = off.second.second;
        char buf[102400] = {0};
        ifs1.seekg(offset,ifs1.beg);
        ifs1.read(buf,len);
        txt = buf;
        int sbeg = txt.find("<url>")+5;
        int send = txt.find("</url>",sbeg);
        url = txt.substr(sbeg,send-sbeg);
        sbeg = txt.find("<title>")+7;
        send = txt.find("</title>",sbeg);
        title = txt.substr(sbeg,send-sbeg);
        sbeg = txt.find("<content>")+9;
        send = txt.find("</content>",sbeg);
        content = txt.substr(sbeg,send-sbeg);
        _pageLib.emplace_back(title,url,content);
    }
    ifs1.close();

    _conf->getConfigMap();
    _stopWordsLib = _conf->getStopWordList();
}
string WordQuery::doQuery()
{
    loadLibrary();
    cout << "loadLibrary finished." << endl;
    cppjieba::Jieba jieba(DICT_PATH,
                          HMM_PATH,
                          USER_DICT_PATH,
                          IDF_PATH,
                          STOP_WORD_PATH);
    vector<string> querywordsAll,querywords;
    jieba.Cut(_queryWord,querywordsAll,true);
    cout << "querywordsAll: " << endl;
    cout << querywordsAll << endl;
    for(auto& a:querywordsAll)
    {
        auto it = _stopWordsLib.find(a);
        if(it == _stopWordsLib.end() && (a.size()!=1))
            querywords.push_back(a);
    }
    cout << "after stopWordsLib,querywords: " << endl;
    cout << querywords << endl;  
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
            map<string,double> mm;
            mm[w] = weight;
            resultVector.emplace_back(make_pair(d,mm));
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
}

//thid function no problem
void WordQuery::sendResponce()
{
    string response = doQuery();
    int sz = response.size();
    string message(to_string(sz));
    message.append("\n").append(response);
    _conn->sendInLoop(message);
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
    cout << "test WordQuery::getDocIds " << endl;
    for(auto &it : result)
    {
        cout << it << endl;
    }
    return result;
}
#if 1
string WordQuery::createJson(vector<int>& docIds,vector<string>& querywords)
{
    Json::Value result;
    int maxCnt = 0;
    for(auto id:docIds)
    {
        if(maxCnt > 50)
            break;
        Json::Value tmp;
        WebPage iPage = _pageLib[id-1];
        tmp["title"] = iPage.getTitle();
        tmp["summary"] = iPage.getSummary(querywords); //getSummsary不对
        tmp["url"] = iPage.getUrl();
        tmp["content"] = iPage.getContent();
        result["files"].append(tmp);
        ++maxCnt;
    }
    //Json::FastWriter fast_writer;
    //string response = fast_writer.write(result);
    Json::StyledWriter style_writer;
    return style_writer.write(result);
}
#endif

//this function no problem
string WordQuery::returnNoAnswer()
{
    Json::Value result,tmp;
    tmp["title"] = "404. Not found.";
    tmp["summary"] = "Sorry,I cann't what you want.Please search something else.";
    tmp["url"] = "";
    tmp["content"] = "";
    result["files"].append(tmp);
    //Json::FastWriter fast_writer;
    //string response = fast_writer.write(result);
    Json::StyledWriter style_writer;
    return style_writer.write(result);
}
}//end of namespace wd
