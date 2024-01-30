CC = g++
CFLAGS = -g -Wall -Weffc++ -c

INCLUDE_FILES = include/Action.h include/Customer.h include/Order.h include/Volunteer.h include/WareHouse.h
OBJECTS = bin/main.o bin/Action.o bin/Customer.o bin/Order.o bin/Volunteer.o bin/WareHouse.o


all: build

run: all
	./bin/main bin/configFileExample.txt

build: $(OBJECTS)
	g++ -o bin/main bin/Action.o bin/Customer.o bin/main.o bin/Order.O bin/Volunteer.o bin/WareHouse.o

bin/main.o: src/main.cpp $(OBJECTS) $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

bin/%.o: src/%.cpp $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

valgrind: build
	valgrind --leak-check=yes --show-reachable=yes --log-file="valgrind_report.txt" bin/main bin/configFileExample.txt

clean:
	rm -f bin/*.o
	rm -f bin/main