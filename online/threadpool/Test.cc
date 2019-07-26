#include "../../include/SearchEngine.h"
#include <iostream>

int main()
{
    wd::SearchEngine engine("../../conf/myconf.conf");
    engine.start();
    return 0;
}

