CXXFLAGS += -std=c++11 -Wno-c++11-narrowing
LDFLAGS  += -lboost_thread-mt -lboost_system-mt -pthread

targets = create search server
objects = util

include cppmake.mk
