CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -Isrc
LDFLAGS  = -lGL -lGLEW -lglfw -lm

TARGET  = golf
SRCS    = main.cpp $(wildcard src/*.cpp)
OBJS    = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
