#include "../include/simhash/include/simhash/cppjieba/Jieba.hpp"
#include "PageLibPreprocessor.h"
#include "WebPage.h"

namespace sr
{

const char* const DICT_PATH = "../include/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../include/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../include/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../include/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../include/cppjieba/dict/stop_words.utf8";

bool is_chinese(const string& str)
{
    unsigned char utf[4] = {0};
    unsigned char unicode[3] = {0};
    bool res = false;
    for (int i = 0; i < str.length(); i++) {
        if ((str[i] & 0x80) == 0) {   //ascii begin with 0
            res = false;
        }
        else /*if ((str[i] & 0x80) == 1) */{
            utf[0] = str[i];
            utf[1] = str[i + 1];
            utf[2] = str[i + 2];
            i++;
            i++;
            unicode[0] = ((utf[0] & 0x0F) << 4) | ((utf[1] & 0x3C) >>2);
            unicode[1] = ((utf[1] & 0x03) << 6) | (utf[2] & 0x3F);
            if(unicode[0] >= 0x4e && unicode[0] <= 0x9f){
                if (unicode[0] == 0x9f && unicode[1] >0xa5)
                    res = false;
                else         
                    res = true;
            }else
                res = false;
        }
    }
    return res;
}

PageLibPreprocessor::PageLibPreprocessor(const string& confname)
{
    ifstream ifs;
    ifs.open(confname);
    string xml;
    while(ifs>>xml)
        _xml.push_back(xml);
    ifs.close();
    _rss = *(new vector<WebPage>);
}

void PageLibPreprocessor::parseRss()
{
    for(auto &xml:_xml)
    {
        cout << "cur_xml: " << xml << endl;
        XMLDocument doc;
        int ret = doc.LoadFile(xml.c_str());
        if(0 == ret){
            cout << "LoadFile finished." << endl;
        }else{
            cout << "LoadFile failed." << endl;
        }
        XMLElement* root = doc.RootElement(); 
        XMLElement* channel = root->FirstChildElement("channel");
        if(channel == nullptr){
            cout << "fail to get channel" << endl;
            continue;
        }
        XMLElement* pnode = channel->FirstChildElement("item");
        string title,link,description,content,res;
        int docId = 1;
        regex re("<[^>]*>");
        while(pnode)
        {
            XMLElement* ptitle = pnode->FirstChildElement("title");
            if(ptitle && ptitle->GetText()!= nullptr){
                title = ptitle->GetText();
            }else{
                title = string();
            }

            XMLElement* plink = pnode->FirstChildElement("link");
            if(plink && plink->GetText()!=nullptr){
                link = plink->GetText();
            }else{
                link = string();
            }

            XMLElement* pdescription = pnode->FirstChildElement("description");
            if(pdescription && pdescription->GetText()!=nullptr){
                description = pdescription->GetText();
                description = regex_replace(description,re,"");
            }else{
                description = string();
            }

            XMLElement* pcontent = pnode->FirstChildElement("content:encoded");
            if(pcontent && pcontent->GetText()!=nullptr){
                content = pcontent->GetText();
                content = regex_replace(content,re,"");
            }

            if(title == string()){
                if(description != string()){
                    stringstream ss(description);
                    getline(ss,title);
                }else{
                    if(content != string()){
                        stringstream ss(content);
                        getline(ss,content);
                    }
                }
            }

            pnode = pnode->NextSiblingElement();
            ++docId;
            _rss.emplace_back(docId,title,link,content);
        }
        cout << "extract finished." << endl;
    }
}

void PageLibPreprocessor::pageFilter()
{
    cout << "before pageFilter,the size of _rss: " << _rss.size() << endl;
    int i = 1;
    for (auto& page:_rss)
    {
        page.builU64();
        cout << "page number: " << i << endl;
        ++i;
    }
    sort(_rss.begin(),_rss.end());
    auto it = unique(_rss.begin(),_rss.end());
    _rss.erase(it,_rss.end());
    _N = _rss.size();
    cout << "after pageFilter,the size of _rss" << _N << endl;
}

void PageLibPreprocessor::buildInvertIndex()
{
    cppjieba::Jieba jieba(DICT_PATH,
                          HMM_PATH,
                          USER_DICT_PATH,
                          IDF_PATH,
                          STOP_WORD_PATH);
    vector<string> words;
    string line;
    for(auto& page:_rss)
    {
        map<string,int> iWords;
        line = page.getDoc();
        jieba.Cut(line,words,true);
        for(auto& word:words){
            if(is_chinese(word)){
                ++iWords[word];
            }
        }
        for(auto &w:iWords)
        {
            ++_words[w.first];
        }
        _wordPage.push_back(iWords);
    }
    int docId = 1;
    double tf,df;
    string curWord;
    for(auto& pword:_wordPage)
    {
        double sumWeights = 0;
        set<double> weightLib;
        unordered_map<string,double> perIndex;
        for(auto it = pword.begin();it != pword.end();++it)
        {
            curWord = it->first;
            tf = it->second;
            df = _words.find(curWord)->second;
           // cout << "word: " << curWord << " tf: " << tf << " df: " << df << endl;
            computeWeight(tf,df);
            weightLib.insert(_weight);
            perIndex[curWord] = _weight;
        }
        for(auto &w:weightLib)
        {
            sumWeights += w * w;
        }
        cout << "sumWeights" << sumWeights << endl;
        double newWeight;
        for(auto &ind:perIndex)
        {
            //cout << "old weight: " << ind.second << endl;
            newWeight = ind.second/sqrt(sumWeights);
            //cout << "new weight: " << newWeight << endl;
            _index[ind.first].insert(make_pair(docId,newWeight));
        }
        cout << "docId: " << docId << endl; //***测试信息 
        ++docId;
    }
}

void PageLibPreprocessor::computeWeight(double tf,double df)
{
    double idf = log(_N/df);
    _weight = tf * idf;
}

void PageLibPreprocessor::store(const string& ripeName,const string& offsetName,const string& indexName)
{
    cout << ">>ripeName " << ripeName << endl;
    cout << ">>offsetName" << offsetName << endl;
    cout << ">>indexName" << indexName << endl;
    string txt;
    ofstream ofs1,ofs2,ofs3;
    int offset,len;
    ofs1.open(ripeName);
    ofs2.open(offsetName);
    ofs3.open(indexName);
    int id = 1;
    for(auto &c:_rss)
    {
        txt = c.getDoc();
        ofs1 << txt << endl;
        len = txt.size();
        offset = (int)ofs1.tellp() - len;
        ofs2 << id << " " << offset << " " << len << endl;
        ++id;
    }
    for(auto &i:_index)
    {
        ofs3 << i.first;
        for(auto &j:i.second){
            ofs3 << " " << j.first << " " <<j.second;
        }
        ofs3 << endl;
    }

    cout << "finish to write" << endl;
    ofs1.close();
    ofs2.close();
    ofs3.close();
}
}//end of namespace sr
