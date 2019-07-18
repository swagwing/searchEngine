#include "RssReader.h"

int main()
{
    sr::RssReader objRss;
    objRss.pareRss();
    objRss.dump("./pagelib.txt");
    return 0;
}

