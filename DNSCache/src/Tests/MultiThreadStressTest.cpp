#include <gtest/gtest.h>

#include "DnsCache.h"

#include <unordered_map>
#include <string>
#include <random>
#include <memory>
#include <thread>

void Threadtask(std::vector<std::string> const& data) {
    for (std::string const& name : data) {
        std::string ip = DNSCache::GetInstance().resolve(name);
        // We won't check full resolve result, coz we have built cache data
        // so that last symbols of name and corresponding ip were equal
        EXPECT_EQ(name.back(), ip.back());
    }
}

TEST(MultiThreadStressTest, Case1) {
    size_t const CacheSize = 10;
    size_t const ThreadsCount = 50;
    DNSCache::SetupGlobalParams(CacheSize, ThreadModel::Multiple);
    for (size_t i = 0; i < CacheSize; ++i) {
        DNSCache::GetInstance().update(std::string("site") + std::to_string(i),
                                       std::string("1.2.3.") + std::to_string(i));
    }
    std::vector<std::string> names;
    names.reserve(CacheSize*100);
    for (size_t i = 0; i < CacheSize*10; ++i) {
        names.push_back(std::string("site") + std::to_string(i%CacheSize));
    }

    std::vector<std::vector<std::string>> threadsData(ThreadsCount, names);
    std::random_device rd;
    std::mt19937 g(rd());
    for (auto& names : threadsData) {
        std::shuffle(names.begin(), names.end(), g);
    }
    std::vector<std::unique_ptr<std::thread>> threads(ThreadsCount);
    for (size_t i = 0; i < ThreadsCount; ++i) {
        threads[i] = std::make_unique<std::thread>(Threadtask, std::cref(threadsData[i]));
    }
    for (size_t i = 0; i < ThreadsCount; ++i) {
        threads[i]->join();
    }
}
