cmake_minimum_required(VERSION 3.10.0)

add_executable(BaseTest
    BaseTest.cpp
)

target_link_libraries(BaseTest PUBLIC
        GTest::GTest
        GTest::Main
	DnsCache
)

gtest_add_tests(TARGET BaseTest
                TEST_LIST BaseTest-GTest)


add_executable(MultiThreadStressTest
    MultiThreadStressTest.cpp
)

target_link_libraries(MultiThreadStressTest PUBLIC
        GTest::GTest
        GTest::Main
        DnsCache
)

gtest_add_tests(TARGET MultiThreadStressTest
                TEST_LIST MultiThreadStressTest-GTest)


if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_executable(MultiThreadRaceSingletonTest
        MultiThreadRaceSingletonTest.cpp
    )

    target_link_libraries(MultiThreadRaceSingletonTest PUBLIC
            GTest::GTest
            GTest::Main
            DnsCache
    )

    gtest_add_tests(TARGET MultiThreadRaceSingletonTest
                    TEST_LIST MultiThreadRaceSingletonTest-GTest)
endif()
