#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>
#include <fstream>
#include "Pixel.h"
#include <queue>

struct Node {
    unsigned int x, y;
};


enum Direction {
    EST,
    OUEST,
    NORD,
    SUD,
    PUIT,
    SOURCE
};

class Image {
private:
    unsigned int NbLigne;            
    unsigned int NbColonne;   
    std::vector<Pixel> tab; //tableau 1D
    // ---- Accéder à un pixel par coordonnées 1D----
    Pixel getPixel1D (unsigned int t);

    //fonction vérification que la case du tableau existe
    bool pixelExiste(unsigned int i,unsigned int j);

public:
    // ---- Constructeurs ----
    Image();
    Image(unsigned int l, unsigned int c);

    // ---- Accès aux dimensions ----
    unsigned int getligne() const;
    unsigned int getcolonne() const;

    // ---- Accès aux pixels ----
    unsigned int positionPixel(int i, int j); //Pour connaitre la position dans le tableau 1D

    // ---- Chargement / sauvegarde ----
    bool importePGM(const std::string& nomFichier);
    bool sauvgardePGM(const std::string& nomFichier) const;

    // ---- Debug / affichage ----
    void afficheIntensitie() const; //pour afficher tableau des intensités

    // ---- Capacités ----
    
    //void deplacePixelCourant(Direction dir);


    Pixel& accesEntourage(unsigned int x,unsigned int y,Direction dir);
    
    bool exporterPGM(const std::string& nomFichier, int maxVal);

    void calculCapacitePixel(unsigned int x, unsigned int y, double sigma, double alpha);
    void calculCapacitesImage(double sigma, double alpha);
    bool findPath(std::vector<Direction> &chemin,
                     unsigned int &fx, unsigned int &fy);

    unsigned int calculGoulot(unsigned int fx, unsigned int fy,
                                 const std::vector<Direction>& chemin);
    void afficherChemin(const std::vector<Direction>& chemin);
    void flotMaximal();

    Direction getInverseDirection(Direction dir);
    void appliquerFlot(unsigned int fx, unsigned int fy,
                          const std::vector<Direction>& chemin,
                          unsigned int delta);
    void calculerSegmentation();

};

#endif