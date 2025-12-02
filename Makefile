EXEC = image

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

image: Image.o Pixel.o main.o
	g++ -Wall -Wextra -std=c++17 -o image Image.o Pixel.o main.o

Image.o: Image.cpp Image.h Pixel.h
	g++ -Wall -Wextra -std=c++17 -c Image.cpp -o Image.o

Pixel.o: Pixel.cpp Pixel.h
	g++ -Wall -Wextra -std=c++17 -c Pixel.cpp -o Pixel.o

main.o: main.cpp Image.h Pixel.h
	g++ -Wall -Wextra -std=c++17 -c main.cpp -o main.o

clean:
	rm -f Image.o Pixel.o main.o

mrproper: clean
	rm -f image

rebuild: mrproper image

# Règle phony (Pour pas de pbl de nom)
.PHONY: clean mrproper rebuild