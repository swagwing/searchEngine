#include "../../include/WordQuery.h"
#include "../../include/Thread.h"

namespace wd
{

size_t nBytesCode(const char ch)
{
    if(ch & (1 << 7))
    {
        int nBytes = 1;
        for(int idx = 0;idx != 6; ++idx)
        {
            if(ch & (1 << (6-idx))){
                ++nBytes;
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}

size_t length(const string& str)
{
    size_t ilen = 0;
    for(size_t idx=0; idx!=str.size(); ++idx)
    {
        int nBytes = nBytesCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }
    return ilen;
}

WordQuery::WordQuery(const string& queryWord,const TcpConnectionPtr& conn,MyConf& conf)
    :_conf(conf)
    ,_conn(conn)
    ,_queryWord(queryWord)
{}


}//end of namespace wd
