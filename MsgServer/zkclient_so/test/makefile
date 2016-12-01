# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

C++ 	= g++
LINK 	= $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) -g -Wall -std=c++11 -DPOSIX -std=c++11 -D__PTHREADS__ -D__PTHREADS_MUTEXES__

# OPTIMIZATION
CCFLAGS += -O0

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I./include

C++FLAGS =  $(CCFLAGS)

CPPFILES := main.cpp

CPPFLAGS =-L./lib -lzksdk
CPPFLAGS +=-L./lib -lzkclient
CPPFLAGS +=-L./lib/ -lzookeeper_mt
CPPFLAGS +=-L./lib -ljsoncpp
CPPFLAGS +=-lpthread
	
all: test_zksdk

test_zksdk: $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)
	$(C++) -o test_zksdk $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o) $(CPPFLAGS) $(C++FLAGS)

o_clean:
	rm -rf $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)

clean:
	rm -rf test_zksdk $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)

.SUFFIXES: .cpp .cc .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc
