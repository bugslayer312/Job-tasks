//
//  main.cpp
//  hw
//
//  Created by Bugslayer on 20.12.15.
//  Copyright (c) 2015 Bugslayer. All rights reserved.
//

#include <iostream>
#include "mstring.h"
#include <vector>

using namespace std;

int main(int argc, const char * argv[])
{
    mstring s1("Hello, ");
    mstring s2("World!");
    mstring s3 = s1 + s2;
    cout << s3 << endl;
    vector<mstring> v;
    v.push_back(mstring("mOscoW"));
    v.push_back(mstring("peRm"));
    v.push_back(mstring("ekaTeriNburg"));
    v.push_back(mstring("NovosibirSk"));
    v.push_back(mstring("krAsnoYarsk"));
    sort(v.begin(), v.end());
    
    for (vector<mstring>::reverse_iterator it = v.rbegin(); it != v.rend(); it++) {
        cout << *it << endl;
    }
    
    return 0;
}

