#include <gtest/gtest.h>

#include "DnsCache.h"

TEST(DNSCacheBase, Case1) {
    DNSCache::SetupGlobalParams(3, ThreadModel::Single);
    DNSCache& dnsCache = DNSCache::GetInstance();
    EXPECT_EQ(dnsCache.resolve("ya.ru"), "");
    dnsCache.update("ya.ru", "1.1.1.1");
    EXPECT_EQ(dnsCache.resolve("ya.ru"), "1.1.1.1");
    dnsCache.update("google.ru", "1.1.2.1");
    dnsCache.update("github.com", "1.1.3.1");
    EXPECT_EQ(dnsCache.resolve("github.com"), "1.1.3.1");
    EXPECT_EQ(dnsCache.resolve("ya.ru"), "1.1.1.1");
    dnsCache.update("google.ru", "1.1.2.2");
    dnsCache.update("hh.ru", "1.1.4.1");                // github.com go away from cache here
    EXPECT_EQ(dnsCache.resolve("google.ru"), "1.1.2.2");
    EXPECT_EQ(dnsCache.resolve("hh.ru"), "1.1.4.1");
    EXPECT_EQ(dnsCache.resolve("ya.ru"), "1.1.1.1");
    EXPECT_EQ(dnsCache.resolve("github.com"), "");
    dnsCache.update("google.ru", "1.1.2.3");
    dnsCache.update("rsdn.org", "1.1.5.1");             // hh.ru fo away from cache here
    EXPECT_EQ(dnsCache.resolve("hh.ru"), "");

    // in cache : rsdn.org, google.ru, ya.ru

    EXPECT_NE(dnsCache.resolve("ya.ru"), "");
    EXPECT_NE(dnsCache.resolve("google.ru"), "");
    EXPECT_NE(dnsCache.resolve("rsdn.org"), "");
}
