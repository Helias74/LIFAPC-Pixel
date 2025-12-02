#include "Image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <climits>


int main() {
    Image image;
    
    if (!image.importePGM("test.pgm")) {
        return 1;
    }
    
    std::cout << "Image chargée: " << image.getcolonne() << "x" << image.getligne() << std::endl;

    double sigma = 30.0;
    double alpha = 2.0;
    
    image.calculCapacitesImage(sigma, alpha);
    std::cout << "Capacités calculées (sigma=" << sigma << ", alpha=" << alpha << ")" << std::endl;
    
    image.calculerSegmentation();
    
    image.sauvgardePGM("test_segmente.pgm");
    std::cout << "Résultat sauvegardé dans test_segmente.pgm" << std::endl;
    
    return 0;
}