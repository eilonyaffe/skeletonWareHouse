all: clean compile link cleanObjAndrun

compile:
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude src/*.cpp

link:
	g++ -g -Wall -Weffc++ -std=c++11 -o bin/main *.o 

clean:
	rm -f bin/*.o

cleanObjAndrun:
	rm -f *.o 
	valgrind --leak-check=full --show-reachable=yes bin/main "bin/rest/configFileExample.txt"
