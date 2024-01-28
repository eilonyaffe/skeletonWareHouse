all: clean compile link cleanObjAndrun

compile:
	g++ -g -Wall -Weffc++ -std=c++11 -c -Include src/*.cpp

link:
	g++ -g -Wall -Weffc++ -std=c++11 -o bin/main *.o

clean:
	rm -f bin/rest/*

cleanObjAndrun:
	rm -f *.0
	clear
	valgrind --leak-check=full --show-reachable=yes ./bin/main
