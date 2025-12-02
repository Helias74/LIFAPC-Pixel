#ifndef PIXEL_H
#define PIXEL_H

class Pixel {
public:
    unsigned int intensite;

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
    

    Pixel();
     ~Pixel();

    void affichePixel();

};

#endif