#ifndef __WD_MYCONF_H__
#define __WD_MYCONF_H__
#include <iostream>
#include <map>
#include <set>
#include <fstream>
using namespace std;

namespace wd
{

class MyConf
{
public:
    static MyConf* getInstance(const string&);
    static MyConf* getInstance();
    map<string,string>& getConfigMap();
    set<string> getStopWordList();
private:
    MyConf(const string&);
    ~MyConf() {}
private:
    static MyConf* _pInstance;
    string _filepath;
    map<string,string> _configMap;
    set<string> _stopWordList;
};

}//end of namespace wd

#endif

