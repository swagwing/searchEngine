#include "WebPage.h"

namespace sr
{

WebPage::WebPage(int id,const string& title,const string& url,const string& content)
    :_DocId(id)
    ,_title(title)
    ,_url(url)
     ,_content(content)
{
    _txt = "<doc><docid>"+to_string(_DocId)+
        "</docid><url>"+_url+
        "</url><title>"+_title+
        "</title><content>"+_content+
        "</content></doc>";
}

string WebPage::getDoc()
{
    return _txt;
}

void WebPage::builU64()
{
    Simhasher simhasher("../include/simhash/dict/jieba.dict.utf8",
                        "../include/simhash/dict/hmm_model.utf8",
                        "../include/simhash/dict/idf.utf8",
                        "../include/simhash/dict/stop_words.utf8");
    size_t topN = 5;
    vector<pair<string,double>> res;
    simhasher.extract(_txt,res,topN); //提取关键词序列
    simhasher.make(_txt,topN,_u64); //计算simhash值
}

bool WebPage::operator == (const WebPage& rhs)
{
    return Simhasher::isEqual(_u64,rhs._u64);
}

bool WebPage::operator < (const WebPage& rhs)
{
    return _DocId < rhs._DocId; //对文档按DocId排序
}
}//end of namespace sr
