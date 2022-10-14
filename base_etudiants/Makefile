
FLAGS=-std=c++17 -Wall -Werror -Wpedantic -D_GNU_SOURCE
COMPILER=g++

main: main.cpp student.o parsing.o db.o query.o
	${COMPILER} -o tinydb main.cpp parsing.o student.o db.o query.o ${FLAGS}

run:
	make main && ./tinydb

parsing.o: parsing.c parsing.h student.o
	${COMPILER} -c parsing.c ${FLAGS}

student.o: student.c student.h
	${COMPILER} -c student.c ${FLAGS}

query.o: query.c query.h
	${COMPILER} -c query.c ${FLAGS}

db.o: db.c db.h
	${COMPILER} -c db.c ${FLAGS}

tests: tests/run_tests.py
	./tests/run_tests.py

clean:
	rm logs/*