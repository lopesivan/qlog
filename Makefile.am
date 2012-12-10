include_HEADERS = qlog.hpp

check_PROGRAMS = unittests unittestsmt
unittests_SOURCES = unittests.cpp
unittests_CXXFLAGS = -Wall -Wextra -Weffc++ -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wundef -Wshadow -Wsuggest-attribute=pure -Wsuggest-attribute=const -Winvalid-pch -Wno-multichar
unittests_LDADD = -lUnitTest++

unittestsmt_SOURCES = unittests.cpp unittests2.cpp
unittestsmt_CXXFLAGS = -Wall -Wextra -Weffc++ -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wundef -Wshadow -Wsuggest-attribute=pure -Wsuggest-attribute=const -Winvalid-pch -Wno-multichar -std=c++11 -DTEST_MULTITHREADING
unittestsmt_LDADD = -lUnitTest++
unittestsmt_LDFLAGS = -pthread

TESTS = unittests unittestsmt

EXTRA_DIST = doc
