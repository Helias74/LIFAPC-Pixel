# Nom de l'exécutable
EXEC = image

# Compilateur
CXX = g++

# Options du compilateur
CXXFLAGS = -Wall -Wextra -std=c++17

# Fichiers sources
SRC = Image.cpp Pixel.cpp main.cpp

# Fichiers objets générés automatiquement
OBJ = $(SRC:.cpp=.o)

# Fichiers d'en-tête (dépendances)
HEADERS = Image.h Pixel.h

# Règle de compilation finale
$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ)

# Règle de compilation des .cpp en .o avec dépendances
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Dépendances spécifiques (optionnel mais recommandé)
Image.o: Image.cpp Image.h Pixel.h
Pixel.o: Pixel.cpp Pixel.h
main.o: main.cpp Image.h Pixel.h

# Nettoyage des .o
clean:
	rm -f $(OBJ)

# Nettoyage complet (objets + exécutable)
mrproper: clean
	rm -f $(EXEC)

# Règle pour rebuild complet
rebuild: mrproper $(EXEC)

# Règle phony (pour éviter les conflits avec des fichiers nommés clean, etc.)
.PHONY: clean mrproper rebuild