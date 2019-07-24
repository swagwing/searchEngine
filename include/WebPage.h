#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "../include/simhash/include/simhash/Simhasher.hpp"
using namespace std;
using namespace simhash;

namespace sr
{

class WebPage
{
public:
    WebPage(const string&,const string&,const string&);
    string getDoc(int);
    void builU64();
    bool operator == (const WebPage&);
    bool operator < (const WebPage&);

private:
    string _title;
    string _url;
    string _content;
    string _txt;
    uint64_t _u64 = 0;
};

}//end of namespace sr

