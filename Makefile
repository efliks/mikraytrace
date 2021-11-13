INCLUDE=-I/usr/include/eigen3 -I/usr/include/png++ -I/usr/include/openbabel-2.0 -I. -I./cpptoml/include
FLAGS=-W -Wall -pedantic -fPIC -O2

all: mrtp_cli

mrtp_cli: main.o actors.o mappers.o babel.o texture.o light.o camera.o \
		world.o renderer.o easylogging.o
	g++ $^ -o $@ -fopenmp -lm -lpng -lopenbabel

main.o: main.cpp
	g++ $(FLAGS) $(INCLUDE) -o main.o -c main.cpp

actors.o: actors.cpp
	g++ $(FLAGS) $(INCLUDE) -o actors.o -c actors.cpp

mappers.o: mappers.cpp
	g++ $(FLAGS) $(INCLUDE) -o mappers.o -c mappers.cpp

babel.o: babel.cpp
	g++ $(FLAGS) $(INCLUDE) -o babel.o -c babel.cpp

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

easylogging.o: /usr/include/easylogging++.cc
	g++ $(FLAGS) $(INCLUDE) -o easylogging.o -c /usr/include/easylogging++.cc

.PHONY: clean
clean:
	-rm -f mrtp_cli *.o &>/dev/null
