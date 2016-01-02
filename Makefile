#-----------------------------------------------
# Uncomment exactly one of the lines labelled (A), (B), and (C) below
# to switch between compilation modes.

# (A) Production use (optimized mode)
OPT ?= -O2 -DNDEBUG
# (B) Debug mode, w/ full line-level debugging symbols
# OPT ?= -g2
# (C) Profiling mode: opt, but w/debugging symbols
# OPT ?= -O2 -g2 -DNDEBUG
#-----------------------------------------------

SOURCES = \
	db/db_impl.cpp \
	db/coding.cpp \
	db/crc32c.cpp \
	db/file.cpp \
	db/log_reader.cpp \
	db/log_writer.cpp \
	db/status.cpp

CC=cc
CXX=g++
PLATFORM=OS_LINUX
PLATFORM_LDFLAGS=-pthread
PLATFORM_LIBS=
PLATFORM_CCFLAGS= -fno-builtin-memcmp -pthread -DOS_LINUX 
PLATFORM_CXXFLAGS=-std=c++0x -fno-builtin-memcmp -pthread -DOS_LINUX 
PLATFORM_SHARED_CFLAGS=-fPIC
PLATFORM_SHARED_EXT=so
PLATFORM_SHARED_LDFLAGS=-shared -Wl,-soname -Wl,
PLATFORM_SHARED_VERSIONED=true

CFLAGS += -I. -I./include $(PLATFORM_CCFLAGS) $(OPT)
CXXFLAGS += -I. -I./include $(PLATFORM_CXXFLAGS) $(OPT)

LDFLAGS += $(PLATFORM_LDFLAGS)
LIBS += $(PLATFORM_LIBS)

LIBOBJECTS = $(SOURCES:.cpp=.o)


PROGRAMS = merge_tool

LIBRARY = libeasydb.a

default: all

# Should we build shared libraries?
ifneq ($(PLATFORM_SHARED_EXT),)

ifneq ($(PLATFORM_SHARED_VERSIONED),true)
SHARED1 = libeasydb.$(PLATFORM_SHARED_EXT)
SHARED2 = $(SHARED1)
SHARED3 = $(SHARED1)
SHARED = $(SHARED1)
else
# Update db.h if you change these.
SHARED_MAJOR = 0
SHARED_MINOR = 1
SHARED1 = libeasydb.$(PLATFORM_SHARED_EXT)
SHARED2 = $(SHARED1).$(SHARED_MAJOR)
SHARED3 = $(SHARED1).$(SHARED_MAJOR).$(SHARED_MINOR)
SHARED = $(SHARED1) $(SHARED2) $(SHARED3)
$(SHARED1): $(SHARED3)
	ln -fs $(SHARED3) $(SHARED1)
$(SHARED2): $(SHARED3)
	ln -fs $(SHARED3) $(SHARED2)
endif

$(SHARED3):
	$(CXX) $(LDFLAGS) $(PLATFORM_SHARED_LDFLAGS)$(SHARED2) $(CXXFLAGS) $(PLATFORM_SHARED_CFLAGS) $(SOURCES) -o $(SHARED3) $(LIBS)

endif  # PLATFORM_SHARED_EXT

all: $(SHARED) $(LIBRARY)

clean:
	rm -f $(LIBRARY) $(SHARED)  */*.o */*/*.o  

$(LIBRARY): $(LIBOBJECTS)
	rm -f $@
	$(AR) -rs $@ $(LIBOBJECTS)

merge_tool: merge_tool/main.o $(LIBOBJECTS) 
	$(CXX) $(LDFLAGS) merge_tool/main.o $(LIBOBJECTS) -o $@ $(LIBS)

test_1: test/main.o 
	$(CXX) $(LDFLAGS) test/main.o $(LIBRARY) -o test/$@ 

benchmark: benchmark/benchmark.o 
	$(CXX) $(LDFLAGS) benchmark/benchmark.o $(LIBRARY) -o benchmark/$@ 


.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
