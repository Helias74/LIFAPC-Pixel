# TP9 - Binérisation image avec 2 couleurs

## Description
Binarisation d'image en noir et blanc utilisant l'algorithme de flot maximal sur un graphe de pixels.

## Compilation
```bash
make
```

## Utilisation
```bash
./image
```

Le programme :
1. Charge `test.pgm` (nommé le fichier que l'ont veut appliqué en test.pgm)
2. Calcule les capacités des arcs (paramètres : σ=30, α=2.0)
3. Applique l'algorithme de flot maximal
4. Sauvegarde le résultat dans `test_segmente.pgm`

## Structure du projet
```
├── Image.cpp / Image.h    # Structure image + algorithme de flot
├── Pixel.cpp / Pixel.h    # Structure pixel (intensité, capacités, flots)
├── main.cpp               # Point d'entrée du programme
├── Makefile               # Compilation
└── test.pgm               # Image d'entrée (format PGM ASCII)
```

## Algorithme
1. **Modélisation** : Image → graphe avec source S et puits P
2. **Capacités** : 
   - Entre pixels : `exp(-(I(p)-I(q))²/(2σ²))`
   - S→pixel : `-α·ln(I(p)/255)`
   - Pixel→P : `-α·ln((255-I(p))/255)`
3. **Flot maximal** : BFS itératif pour trouver chemins augmentants
4. **Segmentation** : Pixels connectés à S → noir (0), à P → blanc (255)

## Paramètres
- **σ** : seuil de contraste local (entre pixel voisin)
- **α** : affinité source/puits (entre pixel de référence, puis et source)
- **H** : fixé à 100

## Nettoyage
```bash
make clean      # Supprime les .o
make mrproper   # Supprime .o + exécutable
```

## Binome
Mouhmed ALLOUCHE p2303730
Helias LAHBIB p2309994