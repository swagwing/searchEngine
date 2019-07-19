#include "RssReader.h"

namespace sr
{

RssReader::RssReader()
{
    _rss = *(new vector<RssItem>);
}

void RssReader::pareRss()
{
    XMLDocument doc;
    doc.LoadFile("./mifengtd.xml");
    cout << "LoadFile finished" << endl;
    XMLElement* root = doc.RootElement();
    XMLElement* channel = root->FirstChildElement("channel");
    if(channel == nullptr){
        cout << "fail to get channel" << endl;
    }
    XMLNode *pnode = channel->FirstChildElement("item");
    string title,link,description,content,res;
    regex re("<[^>]*>");
    while(pnode)
    {
        cout << "enter while" << endl;
        RssItem* pRss = new RssItem;
        XMLElement* ptitle = pnode->FirstChildElement("title");
        if(ptitle && ptitle->GetText()!= nullptr){
            title = ptitle->GetText();
        }else{
            title = string();
        }
        cout << "after title" << endl;

        XMLElement* plink = pnode->FirstChildElement("link");
        if(plink && plink->GetText()!=nullptr){
            link = plink->GetText();
        }else{
            link = string();
        }
        pRss->link = link;
        cout << "after link" << endl;

        XMLElement* pdescription = pnode->FirstChildElement("description");
        if(pdescription && pdescription->GetText()!=nullptr){
            description = pdescription->GetText();
            description = regex_replace(description,re,"");
        }else{
            description = string();
        }
        pRss->description = description;
        cout << "after description" << endl;

        XMLElement* pcontent = pnode->FirstChildElement("content:encoded");
        if(pcontent && pcontent->GetText()!=nullptr){
            content = pcontent->GetText();
            content = regex_replace(content,re,"");
        }
        pRss->content = content;
        cout << "after content" << endl;

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

        pnode = pnode->NextSibling();
        _rss.push_back(*pRss);
    }
    cout << "extract finished." << endl;
}

void RssReader::dump(const string& filename)
{
    cout << ">>filename: " << filename << endl;
    ofstream ofs;
    ofs.open(filename);
    int id = 1;
    for(auto &c:_rss)
    {
        ofs << "<doc>" << endl
            << "<docid>" << id << "</docid>" << endl
            << "<title>" << c.title << "</title>" << endl
            << "<link>" << c.link << "</link>" << endl
            << "<description>" << c.description << "</description>" << endl
            << "<content>" << c.content << "</content>" << endl
            <<"</doc>" << endl;
        ++id;
    }
    ofs.close();
}
}//end of namespace sr
