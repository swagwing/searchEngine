#include <iostream>
#include <string>
using namespace std;

int main()
{
    string a = "";
    cout << a.empty() << endl;
    if (a == string())
        cout << "a == string()" << endl;
    cout << "a.size():" << a.size() << endl;
    //string b = NULL;
    string b = " ";
    cout << b;
    cout <<"b.size(): "<< b.size() << endl;
    return 0;
}

