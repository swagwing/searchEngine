#pragma once
#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include "../include/tinyxml2.h"
using namespace std;
using namespace tinyxml2;

namespace sr
{

struct MyPage
{
    string title;
    string url;
    string description;
    string content;
};

class PageLibPreprocessor
{
public:
    PageLibPreprocessor(const string&);
    void parseRss();
    void store(const string&,const string&);
private:
    vector<MyPage> _rss;
    vector<string> _xml;
};

}//end of namespace sr
