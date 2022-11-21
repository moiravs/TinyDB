
FLAGS=-std=c++17 -Wall -Wpedantic -D_GNU_SOURCE -Werror
COMPILER=g++

main: process.o main.cpp student.o parsing.o db.o query.o utils.o 
	${COMPILER} -o SmallDB process.o main.cpp parsing.o student.o db.o query.o utils.o  ${FLAGS}

run:
	make main && ./SmallDB

monitoring:
	chmod u+x monitoring.sh
	bash monitoring.sh

process.o:process.cpp process.hpp
	${COMPILER} -c process.cpp ${FLAGS}

utils.o: utils.cpp utils.hpp
	${COMPILER} -c utils.cpp ${FLAGS}

parsing.o: parsing.cpp parsing.hpp student.o
	${COMPILER} -c parsing.cpp ${FLAGS}

student.o: student.cpp student.hpp
	${COMPILER} -c student.cpp ${FLAGS}

query.o: query.cpp query.hpp
	${COMPILER} -c query.cpp ${FLAGS}

db.o: db.cpp db.hpp
	${COMPILER} -c db.cpp ${FLAGS}

tests: tests/run_tests.py
	./tests/run_tests.py

clean:
	rm -f *.o
	rm -f SmallDB
	rm -f logs/*