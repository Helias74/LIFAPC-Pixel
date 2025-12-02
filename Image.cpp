#include "Image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <climits>

Image::Image() : NbLigne(0), NbColonne(0) {}

Image::Image(unsigned int l, unsigned int c) : NbLigne(l), NbColonne(c) {
    tab.resize(l * c);
}

bool Image::pixelExiste(unsigned int i, unsigned int j) {
    return (i < NbLigne && j < NbColonne);
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

bool Image::importePGM(const std::string& nomFichier) {
    std::ifstream fichier(nomFichier);
    if (!fichier.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << nomFichier << std::endl;
        return false;
    }

    std::string ligne;
    // Ignorer les commentaires et lire P2
    do {
        std::getline(fichier, ligne);
    } while (ligne[0] == '#');
    
    if (ligne != "P2") {
        std::cerr << "Erreur : format PGM non supporté (P2 attendu)" << std::endl;
        return false;
    }

    // Lire dimensions (en ignorant commentaires)
    do {
        std::getline(fichier, ligne);
    } while (ligne[0] == '#');
    
    std::istringstream iss(ligne);
    iss >> NbColonne >> NbLigne;

    // Lire valeur max
    int maxInt;
    do {
        std::getline(fichier, ligne);
    } while (ligne[0] == '#');
    
    std::istringstream iss2(ligne);
    iss2 >> maxInt;

    if (NbColonne <= 0 || NbLigne <= 0 || maxInt != 255) {
        std::cerr << "Erreur : dimensions invalides ou intensité Max != 255" << std::endl;
        return false;
    }

    tab.resize(NbLigne * NbColonne);

    // Lire intensités
    for (unsigned int i = 0; i < NbLigne; i++) {
        for (unsigned int j = 0; j < NbColonne; j++) {
            int val;
            fichier >> val;
            unsigned int pos = positionPixel(i, j);
            tab[pos].intensite = val;
            tab[pos].PosX = j;
            tab[pos].PosY = i;
            // Initialiser flots à 0
            for (int d = 0; d < 6; d++) {
                tab[pos].flot[d] = 0;
            }
        }
    }

    return true;
}

bool Image::sauvgardePGM(const std::string& nomFichier) const {
    std::ofstream file(nomFichier);
    if (!file.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << nomFichier << std::endl;
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
        std::cerr << "Erreur : index " << t << " hors limites" << std::endl;
        return Pixel();
    }
    return tab[t];
}

// CORRECTION: La logique de direction était inversée
Pixel& Image::accesEntourage(unsigned int x, unsigned int y, Direction dir) {
    int nx = x;
    int ny = y;

    // IMPORTANT: i=ligne (y), j=colonne (x)
    if (dir == OUEST && x > 0) {
        nx = x - 1;  // colonne -1
    } else if (dir == EST && x < NbColonne - 1) {
        nx = x + 1;  // colonne +1
    } else if (dir == NORD && y > 0) {
        ny = y - 1;  // ligne -1
    } else if (dir == SUD && y < NbLigne - 1) {
        ny = y + 1;  // ligne +1
    }

    return tab[ny * NbColonne + nx];
}

bool Image::exporterPGM(const std::string& nomFichier) {
    return sauvgardePGM(nomFichier);
}

void Image::calculCapacitePixel(unsigned int x, unsigned int y, double sigma, double alpha) {
    unsigned int pos = positionPixel(y, x);  // ATTENTION: i=y, j=x
    Pixel& p = tab[pos];
    
    int I = p.intensite;

    // Capacités vers SOURCE et PUIT (formules corrigées)
    if (I == 0) {
        p.capacite[SOURCE] = 0;
    } else {
        p.capacite[SOURCE] = (unsigned int)std::lround(-alpha * std::log((double)I / 255.0));
    }

    if (I == 255) {
        p.capacite[PUIT] = 0;
    } else {
        p.capacite[PUIT] = (unsigned int)std::lround(-alpha * std::log((double)(255 - I) / 255.0));
    }

    // Capacités vers voisins
    if (pixelExiste(y - 1, x)) {  // NORD
        Pixel& n = tab[positionPixel(y - 1, x)];
        double diff = (double)(I - n.intensite);
        p.capacite[NORD] = (unsigned int)std::lround(100.0 * std::exp(-(diff * diff) / (2.0 * sigma * sigma)));
    } else {
        p.capacite[NORD] = 0;
    }

    if (pixelExiste(y + 1, x)) {  // SUD
        Pixel& s = tab[positionPixel(y + 1, x)];
        double diff = (double)(I - s.intensite);
        p.capacite[SUD] = (unsigned int)std::lround(100.0 * std::exp(-(diff * diff) / (2.0 * sigma * sigma)));
    } else {
        p.capacite[SUD] = 0;
    }

    if (pixelExiste(y, x - 1)) {  // OUEST
        Pixel& o = tab[positionPixel(y, x - 1)];
        double diff = (double)(I - o.intensite);
        p.capacite[OUEST] = (unsigned int)std::lround(100.0 * std::exp(-(diff * diff) / (2.0 * sigma * sigma)));
    } else {
        p.capacite[OUEST] = 0;
    }

    if (pixelExiste(y, x + 1)) {  // EST
        Pixel& e = tab[positionPixel(y, x + 1)];
        double diff = (double)(I - e.intensite);
        p.capacite[EST] = (unsigned int)std::lround(100.0 * std::exp(-(diff * diff) / (2.0 * sigma * sigma)));
    } else {
        p.capacite[EST] = 0;
    }
}

void Image::calculCapacitesImage(double sigma, double alpha) {
    for (unsigned int y = 0; y < NbLigne; y++) {
        for (unsigned int x = 0; x < NbColonne; x++) {
            calculCapacitePixel(x, y, sigma, alpha);
        }
    }
}

bool Image::findPath(std::vector<Direction> &chemin, unsigned int &fx, unsigned int &fy) {
    unsigned int W = NbColonne;
    unsigned int H = NbLigne;

    std::vector<int> parent(W * H, -1);
    std::queue<Node> Q;

    // Pixels atteignables depuis SOURCE
    for (unsigned int y = 0; y < H; y++) {
        for (unsigned int x = 0; x < W; x++) {
            Pixel &p = tab[positionPixel(y, x)];
            if (p.capacite[SOURCE] > p.flot[SOURCE]) {
                Q.push({x, y});
                parent[positionPixel(y, x)] = SOURCE;
            }
        }
    }

    // BFS
    while (!Q.empty()) {
        Node u = Q.front();
        Q.pop();

        unsigned int pos = positionPixel(u.y, u.x);
        Pixel& P = tab[pos];

        // Test vers PUIT
        if (P.capacite[PUIT] > P.flot[PUIT]) {
            fx = u.x;
            fy = u.y;

            // Reconstruction du chemin
            chemin.clear();
            chemin.push_back(PUIT);

            unsigned int cx = u.x;
            unsigned int cy = u.y;

            while (parent[positionPixel(cy, cx)] != SOURCE) {
                Direction d = (Direction)parent[positionPixel(cy, cx)];
                chemin.push_back(d);

                // Reculer
                if (d == NORD) cy++;
                else if (d == SUD) cy--;
                else if (d == EST) cx--;
                else if (d == OUEST) cx++;
            }

            chemin.push_back(SOURCE);
            std::reverse(chemin.begin(), chemin.end());
            return true;
        }

        // Explorer 4 directions
        static Direction dirList[4] = {NORD, SUD, EST, OUEST};

        for (Direction d : dirList) {
            unsigned int vx = u.x, vy = u.y;
            
            if (d == NORD && vy > 0) vy--;
            else if (d == SUD && vy < H - 1) vy++;
            else if (d == EST && vx < W - 1) vx++;
            else if (d == OUEST && vx > 0) vx--;
            else continue;

            unsigned int posV = positionPixel(vy, vx);

            if (parent[posV] != -1) continue;

            Pixel &pU = tab[pos];
            Pixel &pV = tab[posV];
            
            // Capacité résiduelle = Cap(u,v) - F(u,v) + F(v,u)
            Direction inv = getInverseDirection(d);
            int residuel = (int)pU.capacite[d] - (int)pU.flot[d] + (int)pV.flot[inv];

            if (residuel > 0) {
                parent[posV] = d;
                Q.push({vx, vy});
            }
        }
    }

    return false;
}

unsigned int Image::calculGoulot(unsigned int fx, unsigned int fy, const std::vector<Direction>& chemin) {
    unsigned int bottleneck = UINT_MAX;

    unsigned int cx = fx;
    unsigned int cy = fy;
    
    // Premier arc: vers PUIT
    Pixel &pFinal = tab[positionPixel(cy, cx)];
    unsigned int resPuit = pFinal.capacite[PUIT] - pFinal.flot[PUIT];
    bottleneck = std::min(bottleneck, resPuit);

    for (int i = (int)chemin.size() - 2; i >= 0; i--) {
        Direction d = chemin[i];

        if (d == SOURCE) {
            Pixel &p = tab[positionPixel(cy, cx)];
            unsigned int res = p.capacite[SOURCE] - p.flot[SOURCE];
            bottleneck = std::min(bottleneck, res);
            break;
        }

        if (d == PUIT) continue;

        // Calculer parent
        unsigned int px = cx, py = cy;
        if (d == NORD) py--;
        else if (d == SUD) py++;
        else if (d == EST) px--;
        else if (d == OUEST) px++;

        Pixel &parent = tab[positionPixel(py, px)];
        Pixel &enfant = tab[positionPixel(cy, cx)];
        
        // Selon le sujet: Cap(p,q) - F(p,q) + F(q,p)
        Direction inv = getInverseDirection(d);
        int resid = (int)parent.capacite[d] - (int)parent.flot[d] + (int)enfant.flot[inv];
        
        if (resid > 0) {
            bottleneck = std::min(bottleneck, (unsigned int)resid);
        } else {
            // Pas de capacité résiduelle, ne devrait pas arriver
            return 0;
        }

        cx = px;
        cy = py;
    }

    return bottleneck;
}

void Image::afficherChemin(const std::vector<Direction>& chemin) {
    for (Direction d : chemin) {
        switch(d) {
            case EST: std::cout << "EST "; break;
            case OUEST: std::cout << "OUEST "; break;
            case NORD: std::cout << "NORD "; break;
            case SUD: std::cout << "SUD "; break;
            case SOURCE: std::cout << "SOURCE "; break;
            case PUIT: std::cout << "PUITS "; break;
        }
    }
    std::cout << std::endl;
}

Direction Image::getInverseDirection(Direction dir) {
    switch(dir) {
        case NORD: return SUD;
        case SUD: return NORD;
        case EST: return OUEST;
        case OUEST: return EST;
        case SOURCE: return PUIT;
        case PUIT: return SOURCE;
        default: return NORD;
    }
}

void Image::appliquerFlot(unsigned int fx, unsigned int fy,
                          const std::vector<Direction>& chemin,
                          unsigned int delta) {
    unsigned int cx = fx;
    unsigned int cy = fy;

    // Arc vers PUIT
    tab[positionPixel(cy, cx)].flot[PUIT] += delta;

    // Parcourir à l'envers
    for (int i = (int)chemin.size() - 2; i >= 0; i--) {
        Direction d = chemin[i];

        if (d == SOURCE) {
            tab[positionPixel(cy, cx)].flot[SOURCE] += delta;
            break;
        }

        if (d == PUIT) continue;

        // Calculer parent
        unsigned int px = cx, py = cy;
        if (d == NORD) py--;
        else if (d == SUD) py++;
        else if (d == EST) px--;
        else if (d == OUEST) px++;

        Pixel &parent = tab[positionPixel(py, px)];
        Pixel &enfant = tab[positionPixel(cy, cx)];
        
        Direction inv = getInverseDirection(d);
        
        // Augmenter le flot dans l'arc (parent -> enfant)
        // Si on dépasse la capacité, reporter l'excédent sur l'arc inverse
        unsigned int flot_avant = parent.flot[d];
        unsigned int cap = parent.capacite[d];
        
        if (flot_avant + delta <= cap) {
            // Pas de dépassement
            parent.flot[d] += delta;
        } else {
            // Dépassement: saturer l'arc direct et diminuer l'arc inverse
            unsigned int excedent = (flot_avant + delta) - cap;
            parent.flot[d] = cap;
            
            // Diminuer le flot inverse (qui peut être négatif conceptuellement)
            if (enfant.flot[inv] >= excedent) {
                enfant.flot[inv] -= excedent;
            } else {
                // Si pas assez de flot inverse, on met à 0
                enfant.flot[inv] = 0;
            }
        }

        cx = px;
        cy = py;
    }
}

void Image::flotMaximal() {
    std::vector<Direction> chemin;
    unsigned int iteration = 0;

    std::cout << "Début de l'algorithme de flot maximal..." << std::endl;

    while (true) {
        unsigned int fx, fy;
        bool cheminExiste = findPath(chemin, fx, fy);

        if (!cheminExiste) {
            std::cout << "Flot maximal atteint après " << iteration << " itérations." << std::endl;
            break;
        }

        unsigned int delta = calculGoulot(fx, fy, chemin);

        if (delta == 0) {
            std::cout << "Delta = 0, arrêt." << std::endl;
            break;
        }

        appliquerFlot(fx, fy, chemin, delta);
        iteration++;

        if (iteration % 1000 == 0) {
            std::cout << "Itération " << iteration << std::endl;
        }
    }
}

void Image::calculerSegmentation() {
    flotMaximal();

    std::vector<bool> coteSource(NbLigne * NbColonne, false);
    std::queue<Node> file;

    // Pixels accessibles depuis SOURCE
    for (unsigned int y = 0; y < NbLigne; y++) {
        for (unsigned int x = 0; x < NbColonne; x++) {
            Pixel &p = tab[positionPixel(y, x)];
            if (p.capacite[SOURCE] > p.flot[SOURCE]) {
                coteSource[positionPixel(y, x)] = true;
                file.push({x, y});
            }
        }
    }

    // BFS dans graphe résiduel
    static Direction directions[4] = {NORD, SUD, EST, OUEST};

    while (!file.empty()) {
        Node u = file.front();
        file.pop();

        Pixel &p = tab[positionPixel(u.y, u.x)];

        for (Direction d : directions) {
            unsigned int vx = u.x, vy = u.y;

            if (d == NORD && vy > 0) vy--;
            else if (d == SUD && vy < NbLigne - 1) vy++;
            else if (d == EST && vx < NbColonne - 1) vx++;
            else if (d == OUEST && vx > 0) vx--;
            else continue;

            int residuel = (int)p.capacite[d] - (int)p.flot[d];
            if (residuel <= 0) continue;

            unsigned int posV = positionPixel(vy, vx);

            if (!coteSource[posV]) {
                coteSource[posV] = true;
                file.push({vx, vy});
            }
        }
    }

    // Binarisation: SOURCE=0 (noir), PUIT=255 (blanc)
    for (unsigned int y = 0; y < NbLigne; y++) {
        for (unsigned int x = 0; x < NbColonne; x++) {
            unsigned int pos = positionPixel(y, x);
            if (coteSource[pos]) {
                tab[pos].intensite = 0;    // Noir (sombre)
            } else {
                tab[pos].intensite = 255;  // Blanc (clair)
            }
        }
    }

    std::cout << "Segmentation terminée." << std::endl;
}

int main() {
    Image image;
    
    if (!image.importePGM("test.pgm")) {
        return 1;
    }
    
    std::cout << "Image chargée: " << image.getcolonne() << "x" << image.getligne() << std::endl;
    
    // Paramètres recommandés pour test.pgm
    double sigma = 30.0;
    double alpha = 2.0;
    
    image.calculCapacitesImage(sigma, alpha);
    std::cout << "Capacités calculées (sigma=" << sigma << ", alpha=" << alpha << ")" << std::endl;
    
    image.calculerSegmentation();
    
    image.sauvgardePGM("test_segmente.pgm");
    std::cout << "Résultat sauvegardé dans test_segmente.pgm" << std::endl;
    
    return 0;
}