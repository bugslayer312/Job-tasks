#pragma once

#include <string>
#include <memory>

enum class ThreadModel {
    Single,
    Multiple
};

class DNSCache
{
public:
    ~DNSCache();
    DNSCache(DNSCache const&) = delete;
    DNSCache& operator=(DNSCache const&) = delete;
    DNSCache(DNSCache&&) = delete;
    DNSCache& operator=(DNSCache&&) = delete;

    void update(const std::string& name, const std::string& ip);
    std::string resolve(const std::string& name) const;

    static void SetupGlobalParams(size_t maxSize, ThreadModel threadModel);
    static DNSCache& GetInstance();
private:
    DNSCache(size_t maxSize, ThreadModel tm);

private:
    static size_t s_maxSize;
    static ThreadModel s_threadModel;

    class DNSCacheImpl;
    std::unique_ptr<DNSCacheImpl> m_impl;
};
