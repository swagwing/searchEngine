#pragma once
#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <map>
#include <set>
#include <unordered_map>
#include <cmath>
#include "../include/tinyxml2.h"
#include "WebPage.h"
using namespace std;
using namespace tinyxml2;

namespace sr
{

class PageLibPreprocessor
{
public:
    PageLibPreprocessor(const string&);
    void parseRss();
    void pageFilter();
    void buildInvertIndex();
    void computeWeight(double,double);
    void store(const string&,const string&,const string&);
private:
    int _N; // 文章总数
    vector<WebPage> _rss;
    vector<string> _xml;
    double _weight;
    vector<map<string,int>> _wordPage;
    unordered_map<string,set<pair<int,double>>> _index;
    map<string,int> _words; //int存的是包含改词的文档数目
};

}//end of namespace sr
