all:
	g++ -std=c++11 -Wall -Wextra -pedantic -o PalettePro PalettePro.cpp src/tinyfiledialogs.c -lSDL2 -lSDL2main -lSDL2_image