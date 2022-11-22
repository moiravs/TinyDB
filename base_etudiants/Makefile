CXXFLAGS+=-std=c++17 -fsanitize=leak,address -g -Wall -Wextra -Wpedantic -D_GNU_SOURCE 
#CXX=g++
LDLIBS+=-lpthread -lasan
SOURCES = $(wildcard *.cpp */*.cpp)
HEADERS = $(wildcard *.hpp */*.hpp)

OBJ=db.o student.o queries.o

.PHONY: main
main: smalldb client server

smalldb: smalldb.cpp ${OBJ}
	$(CXX) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)

%.o: %.cpp %.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^

.PHONY: clean
clean:
	-rm *.o
	-rm *.hpp.gch

client: smalldb.cpp common.h ${OBJ}
	$(CXX) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)

server: main.cpp common.h ${OBJ}
	$(CXX) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)
