#include "Pixel.h"
#include <iostream>

Pixel::Pixel() :intensite(0),PosX(0),PosY(0) {
    for (int k = 0; k < 4; ++k) {
        capacite[k] = 0;
        flot[k] = 0;
    }
}


void Pixel::affichePixel(){
    std::cout<<"=== Affichage Pixel ==="<<std::endl;
    std::cout<<"Position X : "<<PosX<<std::endl;
    std::cout<<"Position Y : "<<PosY<<std::endl;
    std::cout<<"Intensité : "<<intensite<<std::endl;
    std::cout<<"=== Fin d'affichage Pixel ==="<<std::endl;
}

Pixel::~Pixel(){};