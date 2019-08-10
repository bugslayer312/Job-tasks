#include <gtest/gtest.h>

#include "DnsCache.h"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <memory>
#include <vector>

class SingletonRaceTest : public testing::Test {
protected:
    void SetUp() override {
        DNSCache::SetupGlobalParams(1, ThreadModel::Multiple);
    }

public:
    void Task() {
        std::unique_lock<std::mutex> mlock(m_mutex);
        m_startCond.wait(mlock, [this](){
            return m_canStart;
        });
        DNSCache::GetInstance().update("ya.ru", "1.2.3.4");
        EXPECT_EQ(DNSCache::GetInstance().resolve("ya.ru"), "1.2.3.4");
    }

    void Run() {
        m_canStart = true;
        m_startCond.notify_all();
    }

protected:

bool m_canStart = false;
std::condition_variable m_startCond;
std::mutex m_mutex;
};

TEST_F(SingletonRaceTest, Case1) {
    size_t const ThreadsCount = 20;
    std::vector<std::unique_ptr<std::thread>> threads;
    threads.resize(ThreadsCount);
    for (size_t i = 0; i < ThreadsCount; ++i) {
        threads[i] = std::make_unique<std::thread>([this]() {
            Task();
        });
    }
    Run();
    for (size_t i = 0; i < ThreadsCount; ++i) {
        threads[i]->join();
    }
}
