#include "RssReader.h"

namespace sr
{

RssReader::RssReader(const string& confname)
{
    ifstream ifs;
    ifs.open(confname);
    string xml;
    while(ifs>>xml)
        _xml.push_back(xml);
    ifs.close();
    _rss = *(new vector<MyPage>);
}

void RssReader::parseRss()
{
    for(auto &xml:_xml)
    {
        cout << "cur_xml: " << xml << endl;
        XMLDocument doc;
        int ret = doc.LoadFile(xml.c_str());
        if(0 == ret){
            cout << "LoadFile failed" << endl;
        }else{
            cout << "LoadFile finished" << endl;
        }
        XMLElement* root = doc.RootElement(); 
        XMLElement* channel = root->FirstChildElement("channel");
        if(channel == nullptr){
            cout << "fail to get channel" << endl;
            continue;
        }
        XMLElement* pnode = channel->FirstChildElement("item");
        string title,link,description,content,res;
        regex re("<[^>]*>");
        while(pnode)
        {
            MyPage* pRss = new MyPage;
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
            pRss->url = link;

            XMLElement* pdescription = pnode->FirstChildElement("description");
            if(pdescription && pdescription->GetText()!=nullptr){
                description = pdescription->GetText();
                description = regex_replace(description,re,"");
            }else{
                description = string();
            }
            pRss->description = description;

            XMLElement* pcontent = pnode->FirstChildElement("content:encoded");
            if(pcontent && pcontent->GetText()!=nullptr){
                content = pcontent->GetText();
                content = regex_replace(content,re,"");
            }
            pRss->content = content;

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
            pRss->title = title;

            pnode = pnode->NextSiblingElement();
            _rss.push_back(*pRss);
        }
        cout << "extract finished." << endl;
    }
}

void RssReader::dump(const string& ripeName,const string& offsetName)
{
    cout << ">>ripeName " << ripeName << endl;
    cout << ">>offsetName" << offsetName << endl;
    string txt;
    ofstream ofs1,ofs2;
    int offset,len;
    ofs1.open(ripeName);
    ofs2.open(offsetName);
    int id = 1;
    for(auto &c:_rss)
    {
        txt = "<doc><docid>"+to_string(id)+
            "</docid><url>"+c.url+
            "</url><title>"+c.title+
            "</title><content>"+c.content+
            "</content></doc>";
        ofs1 << txt;
        len = txt.size();
        offset = (int)ofs1.tellp() - len;
        ofs2 << id << " " << offset << " " << len << endl;
        ++id;
    }
    cout << "finish to write" << endl;
    ofs1.close();
    ofs2.close();
}
}//end of namespace sr
