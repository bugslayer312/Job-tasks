//
//  mstring.h
//  hw
//
//  Created by Bugslayer on 21.12.15.
//  Copyright (c) 2015 Bugslayer. All rights reserved.
//

#ifndef __hw__mstring__
#define __hw__mstring__

class mstring;

mstring operator+(mstring& str1, mstring& str2);

class mstring{
    char* sz_Buf = 0;
    unsigned long len = 0;

private:
    void initWithBuf(const char* buf);
    void freeBuff();
    
public:
    mstring();
    mstring(const char* buf);
    mstring(const mstring& src);
    ~mstring();
    
    unsigned long length() const;
    
    operator const char*() const;
    mstring& operator=(const mstring& rhs);
    bool operator<(const mstring& rhs) const;
    friend mstring operator+(mstring& str1, mstring& str2);
};

#endif /* defined(__hw__mstring__) */
