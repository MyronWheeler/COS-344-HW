CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -Ivendor -Iholes -I.
LDFLAGS  = -lGL -lGLEW -lglfw -lm

TARGET  = golf
SRCS    = $(wildcard *.cpp)
OBJS    = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
