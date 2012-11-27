include_HEADERS = qlog.hpp

check_PROGRAMS = unittests
unittests_SOURCES = unittests.cpp
unittests_CXXFLAGS = -Wall -Wextra -Weffc++ -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wundef -Wshadow -Wsuggest-attribute=pure -Wsuggest-attribute=const -Winvalid-pch -Wno-multichar
unittests_LDADD = -lUnitTest++
TESTS = unittests

EXTRA_DIST = doc
