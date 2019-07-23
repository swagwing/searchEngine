#include "../../include/MyConf.h"

namespace wd
{

MyConf* MyConf::getInstance(const string& filepath)
{
    if(nullptr == _pInstance)
    {
        _pInstance = new MyConf(filepath);
    }
    return _pInstance;
}

MyConf* MyConf::getInstance()
{
    if(_pInstance)
        return _pInstance;
    else
        return nullptr;
}

MyConf::MyConf(const string& filepath)
    :_filepath(filepath)
{}

map<string,string>& MyConf::getConfigMap()
{
    ifstream ifs(_filepath);
    string str1,str2;
    while(ifs >> str1 >> str2){
        _configMap.insert(make_pair(str1,str2));
    }
    return _configMap;
}

set<string> MyConf::getStopWordList()
{
    string stopWordPath = _pInstance->getConfigMap().find("stopPath")->second;
    ifstream ifs(stopWordPath);
    string str;
    while(ifs >> str)
    {
        _stopWordList.insert(str);
    }
    cout << "停用词：" << endl;
    for(auto &it:_stopWordList)
        cout << it << endl;
    return _stopWordList;
}

MyConf* MyConf::_pInstance = nullptr;
}//end of namespace wd
