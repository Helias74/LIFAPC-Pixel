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


Pixel& Image::accesEntourage(unsigned int x,unsigned int y,Direction dir) {
    int nx = x;
    int ny = y;

    if (dir == OUEST && pixelExiste(x - 1, y)) nx = x - 1;
    else if (dir == EST && pixelExiste(x + 1, y)) nx = x + 1;
    else if (dir == NORD && pixelExiste(x, y - 1)) ny = y - 1;
    else if (dir == SUD && pixelExiste(x, y + 1)) ny = y + 1;
    else {
        std::cout << "Direction non valide ou pixel inexistant" << std::endl;
        return tab[y * NbColonne + x]; // retourne pixel original
    }

    return tab[ny * NbColonne + nx]; // retourne le vrai pixel
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



void Image::calculCapacitePixel(unsigned int x, unsigned int y, double sigma, double alpha)
{
    Pixel& p = tab[y*NbColonne + x];

    int I = p.intensite;

    // ----------- CAPACITÉ SOURCE -> pixel -----------
    if (I == 0) p.capacite[SOURCE] = 0;
    else p.capacite[SOURCE] = (int)round(-alpha * log((double)I / 255.0) * 100);

    // ----------- CAPACITÉ pixel -> PUITS -----------
    if (I == 255) p.capacite[PUIT] = 0;
    else p.capacite[PUIT] = (int)round(-alpha * log((double)(255 - I) / 255.0) * 100);

    // ----------- CAPACITÉS vers les voisins -----------

    // NORD
    if (pixelExiste(x - 1, y)) {
        Pixel n = accesEntourage(x, y, NORD);
        p.capacite[NORD] = (unsigned int)round(100.0 * exp(-((double)(I - n.intensite) * (I - n.intensite)) / (2.0 * sigma * sigma)));
    } else p.capacite[NORD] = 0;

    // SUD
    if (pixelExiste(x + 1, y)) {
        Pixel s = accesEntourage(x, y, SUD);
        p.capacite[SUD] = (unsigned int)round(100.0 * exp(-((double)(I - s.intensite) * (I - s.intensite)) / (2.0 * sigma * sigma)));
    } else p.capacite[SUD] = 0;

    // OUEST
    if (pixelExiste(x, y - 1)) {
        Pixel o = accesEntourage(x, y, OUEST);
        p.capacite[OUEST] = (unsigned int)round(100.0 * exp(-((double)(I - o.intensite) * (I - o.intensite)) / (2.0 * sigma * sigma)));
    } else p.capacite[OUEST] = 0;

    // EST
    if (pixelExiste(x, y + 1)) {
        Pixel e = accesEntourage(x, y, EST);
        p.capacite[EST] = (unsigned int)round(100.0 * exp(-((double)(I - e.intensite) * (I - e.intensite)) / (2.0 * sigma * sigma)));
    } else p.capacite[EST] = 0;
}






int main() {
    Image image;
    image.importePGM("test.pgm");
    image.afficheIntensitie();

    // On calcule les capacités autour de la case (1,1)
    image.calculCapacitePixel(1,1,100,15);

    
    Pixel voisin = image.accesEntourage(0,1,EST);//voisin = (1 ,1)
    voisin.affichePixel();

    // On affiche ses capacités qui ont bien été calculées
    std::cout << voisin.capacite[EST] << std::endl;
    std::cout << voisin.capacite[NORD] << std::endl;
    std::cout << voisin.capacite[SUD] << std::endl;
    std::cout << voisin.capacite[OUEST] << std::endl;
    std::cout << voisin.capacite[SOURCE] << std::endl;

    return 0;
}
