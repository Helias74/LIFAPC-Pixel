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
    Pixel& p = tab[y * NbColonne + x];

    int I = p.intensite;

    if (I == 0) p.capacite[SOURCE] = 0;
    else p.capacite[SOURCE] = (int)round(-alpha * log((double)I / 255.0) * 100);

    if (I == 255) p.capacite[PUIT] = 0;
    else p.capacite[PUIT] = (int)round(-alpha * log((double)(255 - I) / 255.0) * 100);


    if (pixelExiste(x - 1, y)) {
        Pixel n = accesEntourage(x, y, NORD);
        p.capacite[NORD] = (unsigned int)round(100.0 * exp(-((double)(I - n.intensite) * (I - n.intensite)) / (2.0 * sigma * sigma)));
    } else p.capacite[NORD] = 0;

    if (pixelExiste(x + 1, y)) {
        Pixel s = accesEntourage(x, y, SUD);
        p.capacite[SUD] = (unsigned int)round(100.0 * exp(-((double)(I - s.intensite) * (I - s.intensite)) / (2.0 * sigma * sigma)));
    } else p.capacite[SUD] = 0;

    if (pixelExiste(x, y - 1)) {
        Pixel o = accesEntourage(x, y, OUEST);
        p.capacite[OUEST] = (unsigned int)round(100.0 * exp(-((double)(I - o.intensite) * (I - o.intensite)) / (2.0 * sigma * sigma)));
    } else p.capacite[OUEST] = 0;

    if (pixelExiste(x, y + 1)) {
        Pixel e = accesEntourage(x, y, EST);
        p.capacite[EST] = (unsigned int)round(100.0 * exp(-((double)(I - e.intensite) * (I - e.intensite)) / (2.0 * sigma * sigma)));
    } else p.capacite[EST] = 0;
}


void Image::calculCapacitesImage(double sigma, double alpha)
{
    for (unsigned int y = 0; y < NbLigne; y++)
    {
        for (unsigned int x = 0; x < NbColonne; x++)
        {
            calculCapacitePixel(x, y, sigma, alpha);
        }
    }
}



bool Image::findPath(std::vector<Direction> &chemin) {

    // Dimensions totales du graphe
    unsigned int W = NbColonne;
    unsigned int H = NbLigne;

    // Pour stocker le parent : direction par laquelle on est arrivé
    std::vector<int> parent(W * H, -1);

    // File BFS
    std::queue<Node> Q;

    // Point de départ : SOURCE (virtuel)
    // On met tous les pixels atteignables par SOURCE dans la file
    for (unsigned int y = 0; y < H; y++) {
        for (unsigned int x = 0; x < W; x++) {

            Pixel &p = tab[positionPixel(x,y)];

            // Cap résiduelle depuis la SOURCE
            if (p.capacite[SOURCE] > p.flot[SOURCE]) {
                Q.push({x,y});
                parent[positionPixel(x,y)] = SOURCE;
            }
        }
    }

    // BFS NORMAL
    while (!Q.empty()) {
        Node u = Q.front(); 
        Q.pop();

        Pixel P = tab[positionPixel(u.x, u.y)];

        // Test vers PUITS (arrivée)
        if (P.capacite[PUIT] > P.flot[PUIT]) {

            // Reconstruire le chemin
            chemin.clear();
            chemin.push_back(PUIT);

            unsigned int cx = u.x;
            unsigned int cy = u.y;

            while (parent[positionPixel(cx,cy)] != SOURCE) {
                Direction d = (Direction)parent[positionPixel(cx,cy)];
                chemin.push_back(d);

                // Reculer dans l’image
                if (d == NORD) cy++;
                else if (d == SUD) cy--;
                else if (d == EST) cx--;
                else if (d == OUEST) cx++;
            }

            chemin.push_back(SOURCE);
            std::reverse(chemin.begin(), chemin.end());
            return true;
        }

        // 4 directions normales
        static Direction dirList[4] = {NORD, SUD, EST, OUEST};

        for (Direction d : dirList) {

            // Trouver voisin
            Pixel v = accesEntourage(u.x, u.y, d);

            // Si voisin n'existe pas → OK accesEntourage te renvoie erreur
            if (!pixelExiste(v.PosX, v.PosY)) continue;

            unsigned int posV = positionPixel(v.PosX, v.PosY);

            // Déjà visité ?
            if (parent[posV] != -1) continue;

            // Résiduel = cap - flot (arc direct)
            Pixel &pU = tab[positionPixel(u.x, u.y)];
            int residuel = (int)pU.capacite[d] - (int)pU.flot[d];

            if (residuel > 0) {
                parent[posV] = d; // On note par quelle direction on y arrive
                Q.push({v.PosX, v.PosY});
            }
        }
    }

    // AUCUN chemin trouvé
    return false;
}



unsigned int Image::calculGoulot(unsigned int px, unsigned int py, const std::vector<int>& parent)
{
    unsigned int bottleneck = UINT_MAX;

    // Position actuelle = pixel menant vers le PUITS
    unsigned int cx = px;
    unsigned int cy = py;

    while (true) {

        int p = parent[positionPixel(cx, cy)];

        // ----- 1) Cas où le parent est la SOURCE -----
        if (p == SOURCE) {

            Pixel &child = tab[positionPixel(cx, cy)];
            unsigned int res = child.capacite[SOURCE] - child.flot[SOURCE];

            bottleneck = std::min(bottleneck, res);
            break;
        }

        // ----- 2) Cas où le parent est le PUITS (normalement jamais ici) -----
        if (p == PUIT) {
            break;
        }

        // ----- 3) Parent = index dans le tableau -----

        unsigned int pxp = p % NbColonne;   // position X du parent
        unsigned int pyp = p / NbColonne;   // position Y du parent

        Pixel &par = tab[p];
        Pixel &cur = tab[positionPixel(cx, cy)];

        // Déterminer la direction (parent → enfant)
        Direction d;

        if (pxp + 1 == cx)      d = EST;
        else if (pxp - 1 == cx) d = OUEST;
        else if (pyp + 1 == cy) d = SUD;
        else                    d = NORD;

        // capacité résiduelle entre parent → enfant
        unsigned int resid = par.capacite[d] - par.flot[d];

        bottleneck = std::min(bottleneck, resid);

        // on monte d’un cran dans le chemin
        cx = pxp;
        cy = pyp;
    }

    return bottleneck;
}

//Fonction non membre (pas ouf de mettre dans classe )
void Image::afficherChemin(const std::vector<Direction>& chemin) {
    for (Direction d : chemin) {
        switch(d) {
            case EST:    std::cout << "EST "; break;
            case OUEST:  std::cout << "OUEST "; break;
            case NORD:   std::cout << "NORD "; break;
            case SUD:    std::cout << "SUD "; break;
            case SOURCE: std::cout << "SOURCE "; break;
            case PUIT:   std::cout << "PUITS "; break;
            //default:     std::cout << "? "; break;
        }
    }
    std::cout << std::endl;
}


Direction Image::getDirection(unsigned int parentId, unsigned int childId) {
    unsigned int px = parentId / NbColonne;
    unsigned int py = parentId % NbColonne;
    unsigned int cx = childId / NbColonne;
    unsigned int cy = childId % NbColonne;

    if (cx == px - 1 && cy == py) return NORD;
    if (cx == px + 1 && cy == py) return SUD;
    if (cx == px && cy == py + 1) return EST;
    if (cx == px && cy == py - 1) return OUEST;

    // arcs spéciaux vers SOURCE ou PUITS
    if (parentId == SOURCE) return SOURCE;
    if (childId == PUIT) return PUIT;

    // Par défaut (erreur)
    std::cerr << "Erreur : direction impossible entre parentId = " 
              << parentId << " et childId = " << childId << std::endl;
    return NORD; // valeur par défaut
}

Direction Image::getInverseDirection(Direction dir) {
    switch(dir) {
        case NORD: return SUD;
        case SUD:  return NORD;
        case EST:  return OUEST;
        case OUEST:return EST;
        case SOURCE: return PUIT; // pour le graphe résiduel, inverse de SOURCE -> pixel
        case PUIT:  return SOURCE; // inverse de pixel -> PUITS
        default:
            std::cerr << "Erreur : direction inconnue pour getInverseDirection" << std::endl;
            return NORD; // valeur par défaut
    }
}



void Image::appliquerFlot(unsigned int finX, unsigned int finY,
                                const std::vector<int>& parent, unsigned int delta)
{
    unsigned int id = positionPixel(finX, finY);

    while (parent[id] != SOURCE && parent[id] != -1) {
        unsigned int parentId = parent[id];

        // Déterminer la direction de parent -> id
        Direction dir = getDirection(parentId, id); 

        // Ajouter le flot sur l’arc
        tab[parentId].flot[dir] += delta;

        // Mettre à jour l’arc inverse pour le graphe résiduel
        Direction dirInverse = getInverseDirection(dir);
        tab[id].flot[dirInverse] -= delta;

        // Remonter dans le chemin
        id = parentId;
    }

    // Enfin, gérer le dernier arc depuis la SOURCE
    if (parent[id] == SOURCE) {
        Direction dir = SOURCE; // arc source->id
        tab[id].flot[dir] += delta;
    }
}


void Image::flotMaximal() {
    std::vector<int> parent(NbLigne * NbColonne, -1); // tableau parent pour BFS
    bool cheminExiste = true;

    while (cheminExiste) {
        // 1️⃣ Trouver un chemin non saturé S -> P
        cheminExiste = findPath(parent); // à coder : BFS remplissant parent[]
        if (!cheminExiste) break;

        // 2️⃣ Calculer le goulot de ce chemin
        unsigned int finX, finY;
        // récupérer les coordonnées du PUITS ou du pixel final du chemin
        getPixelFinal(parent, finX, finY); // fonction à coder selon BFS
        unsigned int delta = calculGoulot(finX, finY, parent);

        // 3️⃣ Appliquer le flot sur tout le chemin
        appliquerFlot(finX, finY, parent, delta);
    }

    std::cout << "Flot maximal appliqué. Tous les chemins S -> P sont saturés." << std::endl;
}





int main() {
    Image image;
    image.importePGM("test.pgm");
    image.calculCapacitesImage(15,15);
    image.afficheIntensitie();
    std::vector<Direction> chemin;
    image.findPath(chemin);
    image.afficherChemin(chemin);


    
    return 0;
}
