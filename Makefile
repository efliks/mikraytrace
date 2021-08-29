INCLUDE=-I/usr/include/eigen3 -I/usr/include/png++ -I. -I./cpptoml/include
FLAGS=-W -Wall -pedantic -fPIC -O2

all: mrtp_cli

mrtp_cli: main.o actors.o texture.o light.o camera.o world.o renderer.o config.o
	g++ $^ -o $@ -fopenmp -lm -lpng

main.o: main.cpp
	g++ $(FLAGS) $(INCLUDE) -o main.o -c main.cpp

actors.o: actors.cpp
	g++ $(FLAGS) $(INCLUDE) -o actors.o -c actors.cpp

texture.o: texture.cpp
	g++ $(FLAGS) $(INCLUDE) -o texture.o -c texture.cpp

light.o: light.cpp
	g++ $(FLAGS) $(INCLUDE) -o light.o -c light.cpp

camera.o: camera.cpp
	g++ $(FLAGS) $(INCLUDE) -o camera.o -c camera.cpp

world.o: world.cpp
	g++ $(FLAGS) $(INCLUDE) -o world.o -c world.cpp

renderer.o: renderer.cpp
	g++ $(FLAGS) -fopenmp $(INCLUDE) -o renderer.o -c renderer.cpp

config.o: config.cpp
	g++ $(FLAGS) $(INCLUDE) -o config.o -c config.cpp

.PHONY: clean
clean:
	-rm -f mrtp_cli *.o &>/dev/null
