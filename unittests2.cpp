#define QLOG_USE_ASSERTS
#include "qlog.hpp"

#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <fstream>

#ifdef TEST_MULTITHREADING
#   include <thread>
#endif

#ifdef TEST_MULTITHREADING
void multithreading_test_one(const char ch, const unsigned maxIter);
void createSomeThreads()
{
    std::thread t1( multithreading_test_one, 'a', 80000);
    std::thread t2( multithreading_test_one, 'b', 80000);

    t1.join();
    t2.join();
}
#endif
