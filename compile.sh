#!/usr/bin/env bash
test -ex

#CXXFLAGS="-Ofast -std=c++11 -Wno-c++11-narrowing"
CXXFLAGS="-g -std=c++11 -Wno-c++11-narrowing"
LDFLAGS="-lboost_thread-mt -lboost_system-mt -pthread"

for f in util create search server ; do
  g++ -o $f.o -c $f.cpp $CXXFLAGS
done

g++ -o create util.o create.o $LDFLAGS
g++ -o search util.o search.o $LDFLAGS
g++ -o server util.o server.o $LDFLAGS

