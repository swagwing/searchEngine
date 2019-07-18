#pragma once
#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include "../../include/tinyxml2.h"
using namespace std;
using namespace tinyxml2;

namespace sr
{
struct RssItem
{ 
    string title;
    string link;
    string description;
    string content;
};

class RssReader
{
public:
    RssReader();
    void pareRss(); //解析
    void dump(const string&);
private:
    vector<RssItem> _rss;
};
}// end of namespace sr
