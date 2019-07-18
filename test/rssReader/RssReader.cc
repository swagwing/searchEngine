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
    doc.LoadFile("./coolshell.xml");
    XMLNode *pinit = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("item");
    XMLNode *pnode = pinit;
    XMLText *ptxt;
    string title,link,description,content,res;
    regex r1("<.*");
    regex r2("<.*?>");
    while(pnode)
    {
        RssItem* pRss = new RssItem;
        ptxt = pnode->FirstChildElement("title")->FirstChild()->ToText();
        title = ptxt->Value();
        pRss->title = title;

        ptxt = pnode->FirstChildElement("link")->FirstChild()->ToText();
        link = ptxt->Value();
        pRss->link = link;

        ptxt = pnode->FirstChildElement("description")->FirstChild()->ToText();
        description = ptxt->Value();
        description = regex_replace(description,r1,"");
        pRss->description = description;

        ptxt = pnode->FirstChildElement("content:encoded")->FirstChild()->ToText();
        content = ptxt->Value();
        content = regex_replace(content,r2,"");
        pRss->content = content;

        pnode = pnode->NextSibling();
        _rss.push_back(*pRss);
    }
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
