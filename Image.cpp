#include "Image.h"
#include <iostream>
#include <fstream>
#include <sstream>


Image::Image() : NbLigne(0), NbColonne(0) {}

Image::Image(unsigned int l, unsigned int c) : NbLigne(l), NbColonne(c) {
    tab.resize(l * c);
}

//rappel i*c + j

bool Image::pixelExiste(unsigned int i, unsigned int j) {
    return (j < NbColonne && i < NbLigne);
}


unsigned int Image::getligne() const {
    return NbLigne;
}

unsigned int Image::getcolonne() const {
    return NbColonne;
}


unsigned int Image::positionPixel(int i, int j) {
    return i * NbColonne + j;
}

// Importer un fichier PGM


bool Image::importePGM(const std::string& nomFichier) {
    std::ifstream fichier(nomFichier);
    if (!fichier.is_open()) {
        std::cerr << "Erreur : impossible d’ouvrir le fichier " << nomFichier << std::endl;
        return false;
    }

    std::string PGM;
    fichier >> PGM;
    if (PGM != "P2") {
        std::cerr << "Erreur : format PGM non supporté (P2 attendu)" << std::endl;
        return false;
    }

    // Lire largeur, hauteur, valeur max
    int maxInt;
    fichier >> NbColonne >> NbLigne >> maxInt;

    if (NbColonne <= 0 || NbLigne <= 0 || maxInt != 255) {
        std::cerr << "Erreur : dimensions invalides ou intensité Max != 255" << std::endl;
        return false;
    }

    tab.resize(NbLigne * NbColonne);

    // Lire intensités dans tab + renseigner valeur coord pixel
    for (unsigned int i = 0; i < NbLigne; i++) {
        for (unsigned int j = 0; j < NbColonne; j++) {
            int val;
            fichier >> val;
            tab[positionPixel(i, j)].intensite = val;
            tab[positionPixel(i, j)].PosX = j; //Vérifier si pas d'inversion
            tab[positionPixel(i, j)].PosY = i; // Idem
        }
    }

    return true;
}


bool Image::sauvgardePGM(const std::string& nomFichier) const {
    std::ofstream file(nomFichier);
    if (!file.is_open()) {
        std::cerr << "Erreur : impossible d’ouvrir le fichier " << nomFichier << std::endl;
        return false;
    }

    file << "P2\n";
    file << NbColonne << " " << NbLigne << "\n";
    file << 255 << "\n";

    for (unsigned int i = 0; i < NbLigne; ++i) {
        for (unsigned int j = 0; j < NbColonne; ++j) {
            file << tab[i * NbColonne + j].intensite << " ";
        }
        file << "\n";
    }

    return true;
}

// ------------------------------------
// Afficher intensités
// ------------------------------------

void Image::afficheIntensitie() const {
    for (unsigned int i = 0; i < NbLigne; ++i) {
        for (unsigned int j = 0; j < NbColonne; ++j) {
            std::cout << tab[i * NbColonne + j].intensite << " ";
        }
        std::cout << "\n";
    }
}

Pixel Image::getPixel1D(unsigned int t) {
    if (t >= tab.size()) {
        std::cerr << "Erreur : index " << t << " hors limites dans getPixel1D()" << std::endl;
        return Pixel(); // renvoie un pixel par défaut (0)
    }
    return tab[t];
}

void Image::deplacePixelCourant(Direction dir) {

    // Vérifier que le pixel courant est valide
    if (!pixelExiste(p.PosX, p.PosY)) {
        std::cout << "Pixel courant invalide, impossible de déplacer." << std::endl;
        return;
    }

    // Déplacements
    if (dir == OUEST) {
        if (pixelExiste(p.PosX- 1, p.PosY )) {
            p.PosX -= 1;
            p.intensite = tab[p.PosY * NbColonne + p.PosX].intensite;
        }
        else {
            std::cout << "Direction non valide pour déplacer le pixel." << std::endl;
        }
    }
    else if (dir == EST) {
        if (pixelExiste(p.PosX + 1, p.PosY )) {
            p.PosX += 1;
            p.intensite = tab[p.PosY * NbColonne + p.PosX].intensite;
        }
        else {
            std::cout << "Direction non valide pour déplacer le pixel." << std::endl;
        }
    }
    else if (dir == NORD) {
        if (pixelExiste(p.PosX , p.PosY - 1)) {
            p.PosY -= 1;
            p.intensite = tab[p.PosY * NbColonne + p.PosX].intensite;
        }
        else {
            std::cout << "Direction non valide pour déplacer le pixel." << std::endl;
        }
    }
    else if (dir == SUD) {
        if (pixelExiste(p.PosX , p.PosY + 1)) {
            p.PosY += 1;
            p.intensite = tab[p.PosY * NbColonne + p.PosX].intensite;
        }
        else {
            std::cout << "Direction non valide pour déplacer le pixel." << std::endl;
        }
    }
    else {
        std::cout << "Direction non valide pour déplacer le pixel." << std::endl;
    }


}

Pixel Image::accesEntourage(Direction dir) {
    Pixel voisin = p;

    if (!pixelExiste(voisin.PosX, voisin.PosY)) {
        std::cout << "Pixel courant invalide" << std::endl;
        return voisin;
    }

    if (dir == OUEST) {
        if (pixelExiste(voisin.PosX- 1, voisin.PosY )) {
            voisin.PosX -= 1;
        }
    }
    else if (dir == EST) {
        if (pixelExiste(voisin.PosX + 1, voisin.PosY )) {
            voisin.PosX += 1;
        }
    }
    else if (dir == NORD) {
        if (pixelExiste(voisin.PosX , voisin.PosY - 1)) {
            voisin.PosY -= 1;
        }
    }
    else if (dir == SUD) {
        if (pixelExiste(voisin.PosX , voisin.PosY + 1)) {
            voisin.PosY += 1;
        }
    }
    else {
        std::cout << "Direction non valide pour un voisin" << std::endl;
    }

    return voisin;
}



bool Image::exporterPGM(const std::string& nomFichier, int maxVal = 255)
{
    std::ofstream fichier(nomFichier);
    if (!fichier.is_open()) {
        std::cerr << "Erreur : impossible de créer le fichier PGM." << std::endl;
        return false;
    }

    // --- Écriture de l’en-tête PGM ---
    fichier << "P2\n";
    fichier << NbColonne << " " << NbLigne << "\n";
    fichier << maxVal << "\n";

    // --- Écriture des pixels depuis le tableau 1D tab ---
    for (unsigned int y = 0; y < NbLigne; y++) {
        for (unsigned int x = 0; x < NbColonne; x++) {
            int index = y * NbColonne + x;  // calcul de l'index dans le tableau 1D
            fichier << tab[index].intensite;
            if (x < NbColonne - 1) fichier << " ";
        }
        fichier << "\n";
    }

    fichier.close();
    return true;
}





int main (){
    Image image;
    image.importePGM("test.pgm");
    image.afficheIntensitie();
    image.p.affichePixel();
}