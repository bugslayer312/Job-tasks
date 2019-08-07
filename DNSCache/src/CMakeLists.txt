cmake_minimum_required (VERSION 3.10.0)
project (DNSCacheProj)

set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
if (NOT BUILD_TYPE)
    set(BUILD_TYPE "Default")
endif()


set(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/../output/${BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT}/lib")
set (CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")

include_directories(include)

#enable_testing()
#include(CTest)
#find_package(GTest REQUIRED)

add_library(DnsCache STATIC
    DnsCache.cpp
)

add_executable(DnsCacheApp
    DnsCacheApp.cpp
)

target_link_libraries(DnsCacheApp
    PUBLIC DnsCache
)
