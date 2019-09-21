src = $(wildcard src/*.cpp)
obj = $(src:.cpp=.o)

LDFLAGS = -Wall -lGL -lGLEW -lglfw

dibr: $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS)