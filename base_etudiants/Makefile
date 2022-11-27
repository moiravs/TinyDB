CXXFLAGS+=-std=c++17 -fsanitize=leak,address -g -Wall -Wextra -Wpedantic -D_GNU_SOURCE 
#CXX=g++
LDLIBS+=-lpthread -lasan
SOURCES = $(wildcard *.cpp */*.cpp)
HEADERS = $(wildcard *.hpp */*.hpp)

OBJ=db.o student.o queries.o signalshandler.o

.PHONY: main
main: smalldb sdbsh

%.o: %.cpp %.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^

.PHONY: clean
clean:
	-rm *.o
	-rm *.hpp.gch

smalldb: smalldb.cpp common.h ${OBJ}
	$(CXX) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)

sdbsh: main.cpp common.h ${OBJ}
	$(CXX) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)
