# Build the assignment executable "main" using GLEW (from Homebrew) and local GLFW
SOURCES := $(wildcard *.cpp)

main: $(SOURCES)
	clang++ -std=c++11 -g \
		-I/opt/homebrew/include \
		-L/opt/homebrew/lib \
		$(SOURCES) \
		dependencies/library/libglfw.3.4.dylib \
		-lGLEW \
		-framework OpenGL \
		-framework Cocoa \
		-framework IOKit \
		-framework CoreVideo \
		-framework CoreFoundation \
		-o main

clean:
	rm -f *.o main

run:
	./main

all:
	make clean
	make
	make run