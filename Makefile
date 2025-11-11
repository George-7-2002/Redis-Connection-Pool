CXX := g++

CXXFLAGS := -std=c++11 -I. -g -Wall


LDFLAGS :=

LDLIBS := -lhiredis -lspdlog -lpthread


TARGET := redis_pool_test


SRCS := \
	main.cc \
	cache_pool.cc \
	config_file_reader.cc \
	dlog.cc \
	util.cc \
	lock.cc \
  util.pdu.cc

OBJS := $(SRCS:.cc=.o)


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
