//
//  mstring.cpp
//  hw
//
//  Created by Bugslayer on 21.12.15.
//  Copyright (c) 2015 Bugslayer. All rights reserved.
//

#include "mstring.h"
#include "string.h"
#include <stdlib.h>
#include <ctype.h>

mstring operator+(mstring& str1, mstring& str2){
    mstring result;
    size_t len1 = str1.length();
    size_t len2 = str2.length();
    if (len1 + len2) {
        result.sz_Buf = (char*)malloc((len1+len2+1)*sizeof(char));
        if (result.sz_Buf) {
            if (len1 > 0) {
                strcpy(result.sz_Buf, str1.sz_Buf);
            }
            if (len2 > 0) {
                strcpy(result.sz_Buf+len1, str2.sz_Buf);
            }
        }
        result.len = len1 + len2;
    }
    return result;
}

// mstring

void mstring::initWithBuf(const char* buf){
    size_t l = strlen(buf);
    if (l > 0) {
        sz_Buf = (char*)malloc((l+1)*sizeof(char));
        if (sz_Buf) {
            strcpy(sz_Buf, buf);
            len = l;
        }
    }
}

void mstring::freeBuff(){
    if (sz_Buf) {
        free(sz_Buf);
        sz_Buf = 0;
        len = 0;
    }
}

mstring::mstring(){
    
}

mstring::mstring(const char* buf){
    initWithBuf(buf);
}

mstring::mstring(const mstring& src){
    initWithBuf(src);
}

mstring::~mstring(){
    freeBuff();
}

unsigned long mstring::length() const{
    return len;
}

mstring::operator const char*() const{
    return sz_Buf;
}

mstring& mstring::operator=(const mstring& rhs){
    if (this == &rhs) {
        return *this;
    }
    freeBuff();
    initWithBuf(rhs);
    return *this;
}

bool mstring::operator<(const mstring& rhs) const{
    size_t minl = rhs.length();
    if (len < minl) {
        minl = len;
    }
    const char* buf2 = rhs;
    for (size_t i=0; i < minl; i++) {
        int c1 = tolower(*(sz_Buf+i));
        int c2 = tolower(*(buf2+i));
        if (c1 == c2) {
            continue;
        }
        return c1 < c2;
    }
    return (len == minl);
}