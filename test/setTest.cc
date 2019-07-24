#include <iostream>
#include <set>
using namespace std;

int main()
{
    set<int> datas;
    datas.insert(1);
    datas.insert(2);
    datas.insert(3);
    datas.insert(2);
    for(auto& i:datas)
    {
        cout << i << endl;
    }
    return 0;
}

