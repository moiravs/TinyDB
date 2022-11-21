
FLAGS=-std=c++17 -g -O2 -Wall -Wextra -Werror -Wpedantic -D_GNU_SOURCE
COMPILER=gcc
OBJ=student.o parsing.o db.o query.o log.o dedicatedprocess.o mainprocess.o pipe.o sighandler.o

main: main.cpp ${OBJ}
	${COMPILER} -o tinydb main.cpp ${OBJ} ${FLAGS}

run:
	make main && ./tinydb

parsing.o: parsing.cpp parsing.h student.o
	${COMPILER} -c parsing.cpp ${FLAGS}

student.o: student.cpp student.h
	${COMPILER} -c student.cpp ${FLAGS}

query.o: query.cpp query.h
	${COMPILER} -c query.cpp ${FLAGS}

db.o: db.cpp db.h
	${COMPILER} -c db.cpp ${FLAGS}

log.o: log.cpp log.h
	${COMPILER} -c log.cpp ${FLAGS}

dedicatedprocess.o: process/dedicatedprocess.cpp process/dedicatedprocess.h
	${COMPILER} -c process/dedicatedprocess.cpp ${FLAGS}

mainprocess.o: process/mainprocess.cpp process/mainprocess.h
	${COMPILER} -c process/mainprocess.cpp ${FLAGS}

pipe.o: ipc/pipe.cpp ipc/pipe.h
	${COMPILER} -c ipc/pipe.cpp ${FLAGS}

sighandler.o: ipc/sighandler.cpp ipc/sighandler.h
	${COMPILER} -c ipc/sighandler.cpp ${FLAGS}

tests: tests/run_tests.py
	./tests/run_tests.py

clean:
	rm -f *.o
