#ifndef PIXEL_H
#define PIXEL_H

class Pixel {
public:
    // Intensité du pixel (0..255)
    unsigned int intensite;

    // Capacités des arcs vers voisins :
    unsigned int capacite[6]; 
    unsigned int flot[6];
    unsigned int PosX,PosY;
    
    enum direction {
        EST,
        OUEST,
        NORD,
        SUD,
        PUIT,
        SOURCE
    };
    

    // Constructeur par défaut
    Pixel();

    //Affichage du pixel courant
    void affichePixel();
    
    //Dest par defaut 
    ~Pixel();

};

#endif