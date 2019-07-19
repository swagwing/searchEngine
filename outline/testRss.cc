#include "RssReader.h"

int main()
{
    sr::RssReader rssReader("./config");
    rssReader.parseRss();
    rssReader.dump("newripepage.lib","offset.lib");
    return 0;
}
