#include "DnsCache.h"

#include <unordered_map>
#include <list>
#include <mutex>
#include <atomic>
#include <cassert>

class MutexWrapper {
public:
    MutexWrapper(ThreadModel tm) : m_tm(tm){}
    void lock() {
        if (m_tm == ThreadModel::Multiple) {
            m_mutex.lock();
        }
    }

    void unlock() {
        if (m_tm == ThreadModel::Multiple) {
            m_mutex.unlock();
        }
    }
private:
    ThreadModel const m_tm;
    std::mutex m_mutex;
};

class DNSCache::DNSCacheImpl {
    struct CacheEntry {
        std::string Value;
        std::list<std::string>::iterator Order;
    };

public:
    DNSCacheImpl(size_t maxSize, ThreadModel tm) :
        m_maxSize(maxSize), m_mutex(tm){}

    void update(const std::string& name, const std::string& ip) {
        std::lock_guard<MutexWrapper> lock(m_mutex);
        auto found = m_cache.find(name);
        if (found == m_cache.end()) {
            CheckTruncate();
            m_order.push_front(name);
            m_cache.insert(std::make_pair(name, CacheEntry{ip, m_order.begin()}));
        }
        else {
            found->second.Value = ip;
            OrderToFront(found->second.Order);
        }
    }
    std::string resolve(const std::string& name) {
        std::lock_guard<MutexWrapper> lock(m_mutex);
        static const std::string emptyString;
        auto found = m_cache.find(name);
        if (found == m_cache.end()) {
            return emptyString;
        }
        OrderToFront(found->second.Order);
        return found->second.Value;
    }

private:
    void CheckTruncate() {
        if (m_order.size() == m_maxSize) {
            m_cache.erase(m_order.back());
            m_order.pop_back();
        }
    }
    void OrderToFront(std::list<std::string>::iterator& order) {
        if (order != m_order.begin()) {
            m_order.push_front(*order);
            m_order.erase(order);
            order = m_order.begin();
        }
    }

private:
    size_t const m_maxSize;
    std::unordered_map<std::string, CacheEntry> m_cache;
    std::list<std::string> m_order;
    MutexWrapper m_mutex;
};

static std::atomic<int> DNSCacheInstanceCount(0);

DNSCache::DNSCache(size_t maxSize, ThreadModel tm) :
    m_impl(std::make_unique<DNSCacheImpl>(maxSize, tm))
{
    assert(++DNSCacheInstanceCount == 1);
}

DNSCache::~DNSCache(){}

void DNSCache::update(const std::string& name, const std::string& ip) {
    m_impl->update(name, ip);
}

std::string DNSCache::resolve(const std::string& name) const {
    return m_impl->resolve(name);
}

size_t DNSCache::s_maxSize = 100;
ThreadModel DNSCache::s_threadModel = ThreadModel::Single;

void DNSCache::SetupGlobalParams(size_t maxSize, ThreadModel threadModel)
{
    s_maxSize = maxSize;
    s_threadModel = threadModel;
}

DNSCache& DNSCache::GetInstance() {
    static DNSCache dnsCache(s_maxSize, s_threadModel);
    return dnsCache;
}
