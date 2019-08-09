#include "DnsCache.h"

#include <boost/program_options.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
// #include <stdio.h>
#include <regex>
#include <algorithm>
#include <memory>
#include <random>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace po = boost::program_options;
namespace po_style = po::command_line_style;

typedef unsigned int uint;

std::string const EmptyStr;

static std::mutex g_genMutex;

std::string ResolveFromDNS(std::string const& name) {
    // I did emulation because using ping or nslookup sometimes take too much time
    std::lock_guard<std::mutex> g(g_genMutex);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    static std::unordered_map<std::string, std::string> generatorCache;
    auto found = generatorCache.find(name);
    if (found == generatorCache.end()) {
        std::string newData;
        for (size_t i=0; i < 4; ++i) {
            newData.append(std::to_string(std::rand()%256));
            newData.append(1, '.');
        }
        newData.pop_back();
        generatorCache.emplace(std::make_pair(std::string(name), std::move(newData)));
        found = generatorCache.find(name);
    }
    return found->second;
    /*std::ostringstream ss("ping -c1 -4 ");
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
    return ""; */
}

void Resolve(std::string const& name) {
    std::ostringstream os;
    os << "-= Thread " << std::hex << std::showbase << std::this_thread::get_id() << " =-" << std::endl;
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

void PrintUsage(std::ostream& os, po::options_description const& desc, char* appFullPath) {
    std::string appName(appFullPath);
    size_t lastSlashIt = appName.rfind('/');
    if (lastSlashIt != std::string::npos) {
        appName.erase(0, lastSlashIt+1);
    }
    os << "USAGE: " << std::endl;
    os << appName << " [Options]" << std::endl << desc;
    os << "Options --file and --name are mutual exclusive" << std::endl;
    os << appName << " enter in interactive mode if absent both --file and --name" << std::endl;
}

int RunInteractive() {
    char buff[256];
    char c = ' ';
    while (true) {
        std::cout << "Enter name: ";
        std::cin.getline(buff, 255);
        Resolve(buff);
        do {
            std::cout << "Next? [y/n]: ";
            std::cin >> c;
        } while(c != 'y' || c != 'n');
        if (c == 'y') {
            break;
        }
    }
    return 0;
}

void RunTask(std::vector<std::string> const& names) {
    std::cout << "Thread " << std::hex << std::showbase
              << std::this_thread::get_id() << " started" << std::endl;
    for (std::string const& name : names) {
        Resolve(name);
    }
    std::cout << "Thread " << std::hex << std::showbase
              << std::this_thread::get_id() << " finished" << std::endl;
}

int main(int argc, char *argv[])
{
    po::options_description desc("Options");
    desc.add_options()
        ("threads,t", po::value<uint>()->value_name("count")->default_value(1),
            "Run threads count")
        ("size,s", po::value<uint>()->value_name("num records")->default_value(10),
            "DNS cache size")
        ("file,f", po::value<std::string>()->value_name("file_name"),
            "Read input from file")
        ("names,n", po::value<std::vector<std::string>>()->multitoken()
            ->value_name("site1 ... siteN"), "Names to request their IPs")
        ("help,h", "Print this help message");

    po::variables_map vm;
    try {
        po::store(
            po::command_line_parser(argc, argv)
                .options(desc)
                .style(po_style::unix_style|po_style::short_allow_next)
                .run(), vm);
        po::notify(vm);
        if (vm.count("file") && vm.count("names")) {
            throw po::error("Conflicting options: --file and --name");
        }
        if (vm.count("help")) {
            PrintUsage(std::cout, desc, argv[0]);
            return 0;
        }
    } catch (po::error& poe) {
        std::cerr << poe.what() << std:: endl << std::endl;
        PrintUsage(std::cerr, desc, argv[0]);
        return EXIT_FAILURE;
    }

    uint threadCount = vm["threads"].as<uint>();
    uint cacheSize = vm["size"].as<uint>();
    bool isInteractive = !vm.count("file") && !vm.count("names");

    DNSCache::SetupGlobalParams(cacheSize,
        (!isInteractive && threadCount > 1) ? ThreadModel::Multiple : ThreadModel::Single);

    if (isInteractive) {
        return RunInteractive();
    }

    std::vector<std::string> names;
    if (vm.count("file")) {
        std::string const fname = vm["file"].as<std::string>();
        std::ifstream ifs(fname);
        if (ifs) {
            char buff[256];
            while (!ifs.eof()) {
                ifs.getline(buff, 255);
                names.push_back(buff);
                if (names.back().empty()) {
                    names.pop_back();
                }
            }
        }
        else {
            std::cerr << "File " << fname << " not found!" << std::endl;
            return EXIT_FAILURE;
        }
    }
    else {
        names = vm["names"].as<std::vector<std::string>>();
    }

    if (threadCount > 1) {
        std::vector<std::vector<std::string>> namesPacks(threadCount, names);
        std::random_device rd;
        std::mt19937 g(rd());
        for (auto& names : namesPacks) {
            std::shuffle(names.begin(), names.end(), g);
        }
        std::vector<std::unique_ptr<std::thread>> workers(threadCount);
        for (size_t i = 0; i < threadCount; ++i) {
            workers[i] = std::make_unique<std::thread>(RunTask, std::cref(namesPacks[i]));
        }
        for (size_t i = 0; i < threadCount; ++i) {
            workers[i]->join();
        }
    }
    else {
        RunTask(names);
    }

    return 0;
}
