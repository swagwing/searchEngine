#pragma once
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <json/json.h>
using namespace std;

namespace wd
{

class WebPage
{
public:
    WebPage(const string&,const string&,const string&);
    string getTitle();
    string getUrl();
    string getContent();
    string getSummary(const vector<string>&);

private:
    string _title;
    string _url;
    string _content;
};

}//end of namespace wd

