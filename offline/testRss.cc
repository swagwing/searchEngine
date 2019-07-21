#include "PageLibPreprocessor.h"

int main()
{
    sr::PageLibPreprocessor pagepre("./config");
    pagepre.parseRss();
    pagepre.pageFilter();
    pagepre.buildInvertIndex();
    pagepre.store("ripepage.lib","offset.lib","invertIndex.lib");
    return 0;
}
