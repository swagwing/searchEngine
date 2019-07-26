#include "../../include/WebPage.h"

namespace wd
{

WebPage::WebPage(const string& title,const string& url,const string& content)
    :_title(title)
    ,_url(url)
    ,_content(content)
{}

string WebPage::getTitle()
{
    return _title;
}

string WebPage::getUrl()
{
    return _url;
}

string WebPage::getContent()
{
    return _content;
}

string WebPage::getSummary(const vector<string>& queryWords)
{
    string summary,tmp;
    stringstream ss(_content);
    getline(ss,tmp);
    char buf[200];
    bzero(buf,200);
    while(ss.read(buf,150))
    {
        tmp = buf;
        bzero(buf,200);
        for(auto& w:queryWords)
        {
            if(tmp.find(w) != string::npos){
                tmp = "..." + tmp +"...";
                if(summary.size()){
                    summary = summary + "\n" +tmp;
                }
                else
                    summary += tmp;
            }
        }
    }
    return summary;
}
}//end of namespace wd
