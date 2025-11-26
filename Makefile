# Nom de l'exécutable
EXEC = image

# Compilateur
CXX = g++

# Options du compilateur
CXXFLAGS = -Wall -Wextra -std=c++17

# Fichiers sources (pas de main.cpp)
SRC = Image.cpp Pixel.cpp

# Fichiers objets générés automatiquement
OBJ = $(SRC:.cpp=.o)

# Règle de compilation finale
$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ)

# Règle de compilation des .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des .o et de l'exécutable
clean:
	rm -f $(OBJ) $(EXEC)

# Nettoyage complet
mrproper: clean
	rm -f $(EXEC)