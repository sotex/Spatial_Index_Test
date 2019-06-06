all:	testiter testsqlite testgeos testgeos2

testiter:	test.hpp testiter.cpp
	$(CXX) -o testiter testiter.cpp -std=c++11 -O2

sqlite3.o:	sqlite3.h sqlite3.c
	$(CC) -c -o sqlite3.o sqlite3.c -I. -DSQLITE_ENABLE_RTREE=1 -DSQLITE_THREADSAFE=0 -O2

testsqlite: test.hpp testsqlite.cpp sqlite3.o
	$(CXX) -o testsqlite testsqlite.cpp sqlite3.o -I. -std=c++11 -O2 -ldl

testgeos:	test.hpp testgeos.cpp
	$(CXX) -o testgeos testgeos.cpp -I. -std=c++11 -O2 -lgeos

testgeos2:	test.hpp testgeos2.cpp
	$(CXX) -o testgeos2 testgeos2.cpp -I. -std=c++11 -O2 -lgeos

clean:
	rm testiter testsqlite testgeos testgeos2
