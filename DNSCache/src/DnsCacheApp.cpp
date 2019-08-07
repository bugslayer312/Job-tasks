#include "DnsCache.h"

#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <regex>

std::string const EmptyStr;

std::string ResolveFromDNS(std::string const& name) {
    std::ostringstream ss("ping -c1 -W1 -4 ");
    ss << "ping -c1 -4 " << name << " 2> /dev/null";
    char buff[1024];
    if (FILE *fp = popen(ss.str().c_str(), "r"))
    {
        ss.str("");
        ss.clear();
        while (fgets(buff, sizeof(buff)-1, fp) != NULL) {
            ss << buff;
        }
        pclose(fp);
    }
    std::regex e("\(([[:d:]]{1,3}\.[[:d:]]{1,3}\.[[:d:]]{1,3}\.[[:d:]]{1,3})\)");
    std::smatch m;
    std::string output = ss.str();
    if (std::regex_search(output, m, e)) {
        return m[0];
    }
    return "";
}

void Resolve(std::string const& name) {
    std::ostringstream os;
    os << name << " : ";
    std::string ip = DNSCache::GetInstance().resolve(name);
    if (ip == EmptyStr) {
        os << "not found in DNS cache" << std::endl;
        os << "Resolving from DNS server: ";
        std::string fromDns = ResolveFromDNS(name);
        if (fromDns == EmptyStr) {
            os << "Fail";
        }
        else {
            os << fromDns << std::endl;
            DNSCache::GetInstance().update(name, fromDns);
            os << "Updating record in DNS cache" << std::endl;
        }
    }
    else {
        os << ip << std::endl;
    }
    os << "----------" << std::endl;
    std::cout << os.str();
}

int main(int argc, char *argv[])
{
    DNSCache::SetupGlobalParams(5, ThreadModel::Single);
    char buff[256];
    while (true) {
        std::cout << "Enter name: ";
        std::cin.getline(buff, 255);
        Resolve(buff);
    }

    return 0;
}
