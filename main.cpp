#include <ncurses.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio> 
#include <fstream>  
#include <sstream>  
#include <algorithm> 
#include <ctime> // Necesar pentru Happy Hour si verificari de calendar

using namespace std;

enum StareAplicatie { MENIU_PRINCIPAL, BILETE, SELECTIE_LOCURI, BAR, PROMOTII, COS };

struct ProdusBar {
    string nume;
    float pret;
    ProdusBar(string n, float p) : nume(n), pret(p) {}
};

struct Promotie {
    string titlu;
    string descriere;
    Promotie(string t, string d) : titlu(t), descriere(d) {}
};

struct ProdusCos {
    string nume;
    float pret;
    ProdusCos(string n, float p) : nume(n), pret(p) {}
};

#include "Film.h" 
#include "Sala.h" 
#include "Proiectie.h"

// --- MASCA STANGA (Jason) ---
void deseneazaMascaHorror(int y, int x, int culoare) {
    attron(COLOR_PAIR(culoare) | A_BOLD); 
    mvprintw(y,      x, "             __________       ");
    mvprintw(y + 1,  x, "          .-'     OO    '-. ");
    mvprintw(y + 2,  x, "        .'   O          O  '.");
    mvprintw(y + 3,  x, "       / O   O  O    O  O   O\\");  
    mvprintw(y + 4,  x, "      | O    __  \\__/  __    O|");  
    mvprintw(y + 5,  x, "      |   O |  |      |  | O  |");
    mvprintw(y + 6,  x, "      |     |__|      |__|    |");
    mvprintw(y + 7,  x, "      |  O        __        O |");
    mvprintw(y + 8,  x, "      |    O     |__|     O   |");
    mvprintw(y + 9,  x, "      |  O                  O |");
    mvprintw(y + 10, x, "      |         O    O        |");
    mvprintw(y + 11, x, "      |  O      O    O      O |");
    mvprintw(y + 12, x, "       \\   O    O    O    O  /");  
    mvprintw(y + 13, x, "        '.                 .'  ");
    mvprintw(y + 14, x, "          '-.___________.-'     ");
    attroff(COLOR_PAIR(culoare) | A_BOLD);
}

// --- MASCA DREAPTA (Noua ta Masca Teatrala) ---
void deseneazaMascaNoua(int y, int x, int culoare) {
    attron(COLOR_PAIR(culoare) | A_BOLD);
    mvprintw(y,      x, "            ____________            ");
    mvprintw(y + 1,  x, "         .-'  ________  '-.         ");
    mvprintw(y + 2,  x, "       .'  .-'        '-.  '.       ");
    mvprintw(y + 3,  x, "      /  .'              '.  \\      ");
    mvprintw(y + 4,  x, "     |  (    :'.    .':    )  |     ");
    mvprintw(y + 5,  x, "     |  /  ..' :    : '..  \\  |     ");
    mvprintw(y + 6,  x, "     | (  '..-'      '-..'  ) |     ");
    mvprintw(y + 7,  x, "     |  \\       (/\\)       /  |   ");
    mvprintw(y + 8,  x, "     |  |      ______      |  |     ");
    mvprintw(y + 9,  x, "     |   \\    (      )    /   |     ");
    mvprintw(y + 10, x, "     |    |   |      |   |    |     ");
    mvprintw(y + 11, x, "     |     \\  \\      /  /     |     ");
    mvprintw(y + 12, x, "     |      | '.    .' |      |     ");
    mvprintw(y + 13, x, "      \\      \\ |    | /      /      ");
    mvprintw(y + 14, x, "       '.     | |  | |     .'       ");
    mvprintw(y + 15, x, "         '-.__| '..' |__.-'         ");
    mvprintw(y + 16, x, "               '.__.'                ");
    attroff(COLOR_PAIR(culoare) | A_BOLD);
}

void afiseazaAntet(int startX) {
    attron(COLOR_PAIR(2) | A_BOLD); 
    mvprintw(0, startX + 3, "  ____ _            __  __           _    ");
    mvprintw(1, startX + 3, " / ___(_)_ __   ___|  \\/  | __ _ ___| | __");
    mvprintw(2, startX + 3, "| |   | | '_ \\ / _ \\ |\\/| |/ _` / __| |/ /");
    mvprintw(3, startX + 3, "| |___| | | | |  __/ |  | | (_| \\__ \\   < ");
    mvprintw(4, startX + 3, " \\____|_|_| |_|\\___|_|  |_|\\__,_|___/_|\\_\\");
    attroff(COLOR_PAIR(2) | A_BOLD); 

    attron(COLOR_PAIR(1)); 
    mvprintw(5, startX, "=================================================");
    attroff(COLOR_PAIR(1));
}

void deseneazaButonD(int y, int x, string text, int culoare, int latime) {
    attron(COLOR_PAIR(culoare) | A_BOLD);
    mvprintw(y, x, "+");
    for(int i = 0; i < latime - 2; i++) printw("-");
    printw("+");

    mvprintw(y + 1, x, "|");
    int spatiiLibere = (latime - 2) - text.length(); 
    int spatiiStanga = spatiiLibere / 2;
    int spatiiDreapta = spatiiLibere - spatiiStanga;
    for(int i = 0; i < spatiiStanga; i++) printw(" "); 
    printw("%s", text.c_str());                   
    for(int i = 0; i < spatiiDreapta; i++) printw(" ");
    printw("|");
    
    mvprintw(y + 2, x, "+");
    for(int i = 0; i < latime - 2; i++) printw("-");
    printw("+");
    attroff(COLOR_PAIR(culoare) | A_BOLD);
}

void deseneazaCasutaPromotie(int y, int x, string titlu, string descriere, int culoare, int latime) {
    attron(COLOR_PAIR(culoare) | A_BOLD);
    mvprintw(y, x, "+");
    for(int i = 0; i < latime - 2; i++) printw("-");
    printw("+");

    mvprintw(y + 1, x, "|");
    int padT1 = (latime - 2 - titlu.length()) / 2;
    int padT2 = (latime - 2 - titlu.length()) - padT1;
    for(int i=0; i<padT1; i++) printw(" ");
    printw("%s", titlu.c_str());
    for(int i=0; i<padT2; i++) printw(" ");
    printw("|");

    mvprintw(y + 2, x, "|");
    for(int i = 0; i < latime - 2; i++) printw(".");
    printw("|");

    mvprintw(y + 3, x, "|");
    int padD1 = (latime - 2 - descriere.length()) / 2;
    int padD2 = (latime - 2 - descriere.length()) - padD1;
    for(int i=0; i<padD1; i++) printw(" ");
    printw("%s", descriere.c_str());
    for(int i=0; i<padD2; i++) printw(" ");
    printw("|");

    mvprintw(y + 4, x, "+");
    for(int i = 0; i < latime - 2; i++) printw("-");
    printw("+");
    attroff(COLOR_PAIR(culoare) | A_BOLD);
}

void deseneazaEcranCinema(int y, int x) {
    attron(COLOR_PAIR(1) | A_BOLD); 
    mvprintw(y, x,     "+------------------------------------------------+");
    mvprintw(y + 1, x, "|                   E C R A N                    |");
    mvprintw(y + 2, x, "+------------------------------------------------+");
    attroff(COLOR_PAIR(1) | A_BOLD);
}

bool esteLocSelectat(int i, int j, const vector<pair<int, int>>& lista) {
    for (auto loc : lista) {
        if (loc.first == i && loc.second == j) return true;
    }
    return false;
}

int main() {
    initscr();
    cbreak();             
    noecho();             
    keypad(stdscr, TRUE); 
    curs_set(0);          

    start_color();
    use_default_colors(); 
    init_pair(1, COLOR_CYAN, -1);     
    init_pair(2, COLOR_YELLOW, -1);   
    init_pair(3, COLOR_GREEN, -1);    
    init_pair(4, COLOR_RED, -1);      
    init_pair(5, COLOR_WHITE, -1);    
    init_pair(6, COLOR_MAGENTA, -1);  

    mousemask(BUTTON1_PRESSED | BUTTON1_CLICKED | BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);
    printf("\033[?1000h\n"); 

    vector<Sala> complexCinema;
    complexCinema.push_back(Sala("IMAX", "Sala IMAX", "sala_imax.txt"));
    complexCinema.push_back(Sala("VIP",  "Sala VIP", "sala_vip.txt", true));
    complexCinema.push_back(Sala("STD1", "Sala Standard 1", "sala_std1.txt"));
    complexCinema.push_back(Sala("STD2", "Sala Standard 2", "sala_std2.txt"));

    // Citire Filme si Gen din fisier text
    vector<Proiectie> programAzi;
    ifstream fisierFilme("filme.txt");
    if (fisierFilme.is_open()) {
        string linie;
        while (getline(fisierFilme, linie)) {
            if (linie.empty()) continue;
            stringstream ss(linie);
            string t, tokenDurata, tokenPret, tokenPremiera, idSalaCautat, oraRulare, g;
            
            getline(ss, t, '|');
            getline(ss, tokenDurata, '|');
            getline(ss, tokenPret, '|');
            getline(ss, tokenPremiera, '|');
            getline(ss, idSalaCautat, '|'); 
            getline(ss, oraRulare, '|');    
            getline(ss, g, '|'); // NOU: Preluam si genul filmului
            
            int d = atoi(tokenDurata.c_str());
            float p = atof(tokenPret.c_str());
            bool premiera = (tokenPremiera == "1");
            
            Film filmNou(t, d, p, premiera, g);
            
            Sala salaAsignata = complexCinema[0]; 
            for (auto s : complexCinema) {
                if (s.getId() == idSalaCautat) {
                    salaAsignata = s;
                    break;
                }
            }
            programAzi.push_back(Proiectie(filmNou, salaAsignata, oraRulare));
        }
        fisierFilme.close();
    }
    if (programAzi.empty()) {
        programAzi.push_back(Proiectie(Film("Eroare", 120, 20.00, false, "Action"), complexCinema[0], "12:00"));
    }

    // Citire Produse Bar din fisier
    vector<ProdusBar> meniuBar;
    ifstream fisierBar("bar.txt");
    if (fisierBar.is_open()) {
        string linie;
        while (getline(fisierBar, linie)) {
            if (linie.empty()) continue;
            stringstream ss(linie);
            string numeProdus, tokenPretBar;
            getline(ss, numeProdus, '|');
            getline(ss, tokenPretBar, '|');
            meniuBar.push_back(ProdusBar(numeProdus, atof(tokenPretBar.c_str())));
        }
        fisierBar.close();
    }
    // NOU: Ne asiguram ca Pachetul Familiei este inclus in meniul barului ca si Combo
    bool arePachetFamilie = false;
    for(auto prod : meniuBar) {
        if(prod.nume.find("PACHETUL FAMILIEI") != string::npos) arePachetFamilie = true;
    }
    if(!arePachetFamilie) {
        meniuBar.push_back(ProdusBar("Combo: PACHETUL FAMILIEI", 160.00));
    }

    // Citire Promotii
    vector<Promotie> listaPromotii;
    ifstream fisierPromotii("promotii.txt");
    if (fisierPromotii.is_open()) {
        string linie;
        while (getline(fisierPromotii, linie)) {
            if (linie.empty()) continue;
            stringstream ss(linie);
            string titluPromotie, descrierePromotie;
            getline(ss, titluPromotie, '|');
            getline(ss, descrierePromotie, '|');
            listaPromotii.push_back(Promotie(titluPromotie, descrierePromotie));
        }
        fisierPromotii.close();
    }

    bool ruleaza = true;
    MEVENT event; 
    StareAplicatie stareCurenta = MENIU_PRINCIPAL;
    StareAplicatie stareAnterioara = MENIU_PRINCIPAL; 
    int indexSelectat = -1; 
    
    int offsetFilme = 0, maxFilmePeEcran = 3; 
    int offsetBar = 0, maxRanduriBar = 3; 
    int offsetPromotii = 0, maxPromotiiPeEcran = 2; 
    int offsetCos = 0, maxCosPeEcran = 5; 

    vector<ProdusCos> cos;
    vector<pair<int, int>> locuriSelectateCurent;

    while (ruleaza) {
        int xLogo = (COLS - 49) / 2; if (xLogo < 0) xLogo = 0; 
        int xButon = (COLS - 28) / 2; if (xButon < 0) xButon = 0;
        int latimeButonFilm = 74; int xButonFilm = (COLS - latimeButonFilm) / 2;
        int latimeCupon = 70; int xCupon = (COLS - latimeCupon) / 2;

        clear();
        
        if (stareCurenta != COS) {
            float totalCos = 0.0;
            for(auto p : cos) totalCos += p.pret;
            char bufferCos[50];
            snprintf(bufferCos, sizeof(bufferCos), "COS [%d] - %.2f RON", (int)cos.size(), totalCos);
            int xCosBtn = COLS - 32;
            if (xCosBtn > 0) deseneazaButonD(1, xCosBtn, string(bufferCos), 2, 30);
        }

        if (stareCurenta == MENIU_PRINCIPAL) {
            afiseazaAntet(xLogo);
            if (xButon >= 34 && (xButon + 28 + 40) < COLS) {
                deseneazaMascaHorror(6, xButon - 34, 4); 
                deseneazaMascaNoua(5, xButon + 32, 1);   
            }
            deseneazaButonD(7, xButon, "1. Cumpara Bilete", 3, 28);   
            deseneazaButonD(11, xButon, "2. Meniu Bar", 3, 28);       
            deseneazaButonD(15, xButon, "3. Oferte si Promotii", 3, 28); 
            deseneazaButonD(19, xButon, "0. Iesire", 4, 28); 
            
            attron(COLOR_PAIR(1));
            mvprintw(23, xLogo + 6, "-> Alege o optiune folosind mouse-ul");
            attroff(COLOR_PAIR(1));
        }
        else if (stareCurenta == BILETE) {
            afiseazaAntet(xLogo);
            if (xButonFilm >= 34 && (xButonFilm + latimeButonFilm + 40) < COLS) {
                deseneazaMascaHorror(7, xButonFilm - 34, 4); 
                deseneazaMascaNoua(6, xButonFilm + latimeButonFilm + 4, 1);
            }
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(7, xLogo - 2, "[ SECTIUNEA BILETE - PROGRAMUL MULTIPLEXULUI ]");
            attroff(COLOR_PAIR(3) | A_BOLD);
            
            for(int i = 0; i < maxFilmePeEcran; i++) {
                int indexReal = offsetFilme + i;
                if(indexReal >= programAzi.size()) break; 
                string tagPremiera = programAzi[indexReal].getFilm().getEstePremiera() ? " [PREMIERA]" : "";
                string tagVIP = programAzi[indexReal].getSala().getEsteVIP() ? " [VIP]" : "";
                
                // Pretul se calculeaza dinamic (arata 10 lei daca e vineri sau 0 lei daca e vineri 13 horror)
                float pretCalculatDinamice = programAzi[indexReal].getPretLoc(0); 

                int culoareButon = 3; 
                if (programAzi[indexReal].getSala().getEsteVIP()) culoareButon = 2; 
                else if (programAzi[indexReal].getFilm().getEstePremiera()) culoareButon = 6; 

                char buffer[120];
                snprintf(buffer, sizeof(buffer), "[%s] %s%s%s | %s - %.2f RON", 
                         programAzi[indexReal].getOra().c_str(),
                         programAzi[indexReal].getFilm().getTitlu().c_str(), 
                         tagVIP.c_str(), tagPremiera.c_str(),
                         programAzi[indexReal].getSala().getNume().c_str(), pretCalculatDinamice);
                
                deseneazaButonD(9 + (i * 4), xButonFilm, string(buffer), culoareButon, latimeButonFilm);
            }
            int yControls = 21; int xControls = (COLS - 60) / 2; 
            if (offsetFilme > 0) { deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16); }
            deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
            if (offsetFilme + maxFilmePeEcran < programAzi.size()) { deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16); }
        }
        else if (stareCurenta == SELECTIE_LOCURI && indexSelectat != -1) {
            int latimeTotalaSalaCaractere = programAzi[indexSelectat].getSala().getLocuriPeRand() * 6;
            int xSalaCentratDinamice = (COLS - latimeTotalaSalaCaractere) / 2;
            if (xSalaCentratDinamice < 0) xSalaCentratDinamice = 0;
            deseneazaEcranCinema(2, (COLS - 50) / 2);
            
            attron(COLOR_PAIR(2) | A_BOLD);
            string detalii = "Film: " + programAzi[indexSelectat].getFilm().getTitlu() + " (" + programAzi[indexSelectat].getFilm().getGen() + ") | " + programAzi[indexSelectat].getSala().getNume();
            mvprintw(6, (COLS - detalii.length()) / 2, "%s", detalii.c_str());
            attroff(COLOR_PAIR(2) | A_BOLD);

            int startY = 8; 
            for (int i = 0; i < programAzi[indexSelectat].getSala().getRanduri(); i++) {
                float pretRandCurent = programAzi[indexSelectat].getPretLoc(i);
                int culoareLocuriLibere = 3; 
                
                if (programAzi[indexSelectat].getSala().getEsteVIP()) {
                    culoareLocuriLibere = 2; 
                }
                else if (programAzi[indexSelectat].getFilm().getEstePremiera()) {
                    // Preluam data curenta a sistemului si pentru colorare
                    time_t t = time(0); tm* acum = localtime(&t);
                    if (acum->tm_wday != 5) { // Daca NU e vineri, aratam cele 3 zone de pret standard
                        if (i < 5) culoareLocuriLibere = 3;       
                        else if (i < 8) culoareLocuriLibere = 1;  
                        else culoareLocuriLibere = 6;             
                    }
                }

                attron(COLOR_PAIR(culoareLocuriLibere));
                mvprintw(startY + (i * 2), xSalaCentratDinamice - 22, "R%2d (%.0f RON)", i + 1, pretRandCurent); 
                attroff(COLOR_PAIR(culoareLocuriLibere));

                int scaunRealContor = 1; 
                for (int j = 0; j < programAzi[indexSelectat].getSala().getLocuriPeRand(); j++) {
                    int posX = xSalaCentratDinamice + (j * 6);
                    char celula = programAzi[indexSelectat].getSala().getTipCelulă(i, j);
                    if (celula == '.') continue;
                    if (!programAzi[indexSelectat].esteLiber(i, j)) {
                        attron(COLOR_PAIR(4) | A_BOLD); mvprintw(startY + (i * 2), posX, "[ XX ]"); attroff(COLOR_PAIR(4) | A_BOLD);
                        scaunRealContor++;
                    } 
                    else if (esteLocSelectat(i, j, locuriSelectateCurent)) {
                        attron(COLOR_PAIR(5) | A_DIM); mvprintw(startY + (i * 2), posX, "[ SS ]"); attroff(COLOR_PAIR(5) | A_DIM);
                        scaunRealContor++;
                    }
                    else {
                        attron(COLOR_PAIR(culoareLocuriLibere)); mvprintw(startY + (i * 2), posX, "[ %02d ]", scaunRealContor); attroff(COLOR_PAIR(culoareLocuriLibere));
                        scaunRealContor++;
                    }
                }
            }
            int yButoane = startY + (programAzi[indexSelectat].getSala().getRanduri() * 2) + 2;
            deseneazaButonD(yButoane + 1, xButon - 16, "0. Inapoi la Filme", 4, 28);
            if (!locuriSelectateCurent.empty()) { deseneazaButonD(yButoane + 1, xButon + 16, "-> ADAUGA IN COS <-", 3, 28); }
        }
        else if (stareCurenta == BAR) {
            afiseazaAntet(xLogo);
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(7, xLogo + 6, "[ SECTIUNEA BAR - GUSTARI & BAUTURI ]");
            attroff(COLOR_PAIR(3) | A_BOLD);

            int latimeCasuta = 36; int spatiuIntre = 4;
            int xStartGrid = (COLS - ((latimeCasuta * 2) + spatiuIntre)) / 2;

            // Preluam ceasul real pentru logica Happy Hour
            time_t t = time(0);
            tm* acum = localtime(&t);
            int ziuaSaptamanii = acum->tm_wday; // 6 = Sambata
            int oraCurenta = acum->tm_hour;     // Ora curenta (0-23)

            bool esteHappyHourNachos = (ziuaSaptamanii == 6 && oraCurenta >= 14 && oraCurenta < 17);

            for(int r = 0; r < maxRanduriBar; r++) {
                int randReal = offsetBar + r;
                for(int c = 0; c < 2; c++) {
                    int indexProdus = (randReal * 2) + c;
                    if(indexProdus >= meniuBar.size()) break;

                    int culoareProdus = 1; 
                    string numeP = meniuBar[indexProdus].nume;
                    float pretFinalProdus = meniuBar[indexProdus].pret;

                    // --- NOU: Logica de pret Happy Hour pentru Nachos ---
                    if (numeP.find("Nachos") != string::npos && ! (numeP.find("Combo") != string::npos)) {
                        if (esteHappyHourNachos) {
                            pretFinalProdus = pretFinalProdus * 0.50; // Reducere 50%
                            numeP += " [HAPPY HOUR 50%]";
                        }
                    }

                    if (numeP.find("Combo") != string::npos) {
                        culoareProdus = 2; 
                    } else if (numeP.find("Popcorn") != string::npos || numeP.find("Nachos") != string::npos) {
                        culoareProdus = 3; 
                    }

                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "%s: %.2f RON", numeP.c_str(), pretFinalProdus);
                    int xCurent = xStartGrid + (c * (latimeCasuta + spatiuIntre));
                    deseneazaButonD(9 + (r * 4), xCurent, string(buffer), culoareProdus, latimeCasuta); 
                }
            }
            int yControls = 9 + (maxRanduriBar * 4) + 1; int xControls = (COLS - 60) / 2; 
            int maxOffsetBar = ((meniuBar.size() + 1) / 2) - maxRanduriBar;
            if (offsetBar > 0) { deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16); }
            deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
            if (offsetBar < maxOffsetBar) { deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16); }
        }
        else if (stareCurenta == PROMOTII) {
            afiseazaAntet(xLogo);
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(7, xLogo + 5, "[ SECTIUNEA OFERTE SI PROMOTII ]");
            attroff(COLOR_PAIR(3) | A_BOLD);
            for(int i = 0; i < maxPromotiiPeEcran; i++) {
                int indexReal = offsetPromotii + i;
                if(indexReal >= listaPromotii.size()) break; 
                int culoareCupon = (i % 2 == 0) ? 1 : 2; 
                deseneazaCasutaPromotie(9 + (i * 6), xCupon, listaPromotii[indexReal].titlu, listaPromotii[indexReal].descriere, culoareCupon, latimeCupon);
            }
            int yControls = 9 + (maxPromotiiPeEcran * 6); int xControls = (COLS - 60) / 2; 
            if (offsetPromotii > 0) { deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16); }
            deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
            if (offsetPromotii + maxPromotiiPeEcran < listaPromotii.size()) { deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16); }
        }
        else if (stareCurenta == COS) {
            afiseazaAntet(xLogo);
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(7, xLogo + 9, "[ COSUL TAU DE CUMPARATURI ]");
            attroff(COLOR_PAIR(2) | A_BOLD);

            if (cos.empty()) {
                mvprintw(11, xLogo + 15, "Cosul tau este gol!");
            } else {
                for(int i = 0; i < maxCosPeEcran; i++) {
                    int indexReal = offsetCos + i;
                    if(indexReal >= cos.size()) break; 
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "%d. %-35s | %.2f RON", indexReal + 1, cos[indexReal].nume.c_str(), cos[indexReal].pret);
                    deseneazaButonD(9 + (i * 4), xButonFilm, string(buffer), 1, latimeButonFilm);
                    attron(COLOR_PAIR(4) | A_BOLD); 
                    mvprintw(10 + (i * 4), xButonFilm + latimeButonFilm + 2, "[ X ]");
                    attroff(COLOR_PAIR(4) | A_BOLD);
                }
            }
            int yControls = 9 + (maxCosPeEcran * 4); int xControls = (COLS - 60) / 2; 
            int maxOffsetCos = cos.size() - maxCosPeEcran; if (maxOffsetCos < 0) maxOffsetCos = 0;
            if (offsetCos > 0) { deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16); }
            deseneazaButonD(yControls, xControls + 20, "<- Inapoi", 4, 20);
            if (offsetCos < maxOffsetCos) { deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16); }
            if (!cos.empty()) {
                deseneazaButonD(yControls + 4, xButon - 16, "Sterge Tot", 4, 28);
                deseneazaButonD(yControls + 4, xButon + 16, "Finalizeaza Comanda", 3, 28);
            }
        }
        
        refresh(); 
        int actiune = getch();
        if (actiune == KEY_RESIZE) continue; 
        
        if (actiune == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                // SCROLL
                if (stareCurenta == BILETE) {
                    if ((event.bstate & BUTTON4_PRESSED) && offsetFilme > 0) { offsetFilme--; continue; }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetFilme + maxFilmePeEcran < programAzi.size()) { offsetFilme++; continue; }
                }
                else if (stareCurenta == BAR) {
                    int maxOffsetBar = ((meniuBar.size() + 1) / 2) - maxRanduriBar;
                    if ((event.bstate & BUTTON4_PRESSED) && offsetBar > 0) { offsetBar--; continue; }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetBar < maxOffsetBar) { offsetBar++; continue; }
                }
                else if (stareCurenta == COS) {
                    int maxOffsetCos = cos.size() - maxCosPeEcran;
                    if ((event.bstate & BUTTON4_PRESSED) && offsetCos > 0) { offsetCos--; continue; }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetCos < maxOffsetCos) { offsetCos++; continue; }
                }

                // CLICK
                if (event.bstate & BUTTON1_PRESSED || event.bstate & BUTTON1_CLICKED) {
                    if (stareCurenta != COS) {
                        int xCosBtn = COLS - 32;
                        if (event.y >= 1 && event.y <= 3 && event.x >= xCosBtn && event.x <= xCosBtn + 30) {
                            stareAnterioara = stareCurenta; stareCurenta = COS; continue;
                        }
                    }

                    if (stareCurenta == MENIU_PRINCIPAL) {
                        if (event.x >= xButon && event.x <= xButon + 27) {
                            if (event.y >= 7 && event.y <= 9) stareCurenta = BILETE;
                            else if (event.y >= 11 && event.y <= 13) stareCurenta = BAR;
                            else if (event.y >= 15 && event.y <= 17) stareCurenta = PROMOTII;
                            else if (event.y >= 19 && event.y <= 21) ruleaza = false;
                        }
                    }
                    else if (stareCurenta == BILETE) {
                        int yControls = 21; int xControls = (COLS - 60) / 2;
                        if (event.y >= yControls && event.y <= yControls + 2) {
                            if (offsetFilme > 0 && event.x >= xControls && event.x <= xControls + 16) { offsetFilme--; }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = MENIU_PRINCIPAL; offsetFilme = 0; }
                            else if (offsetFilme + maxFilmePeEcran < programAzi.size() && event.x >= xControls + 44 && event.x <= xControls + 60) { offsetFilme++; }
                        }
                        else {
                            for(int i = 0; i < maxFilmePeEcran; i++) {
                                int indexReal = offsetFilme + i;
                                if(indexReal >= programAzi.size()) break;
                                int yFilm = 9 + (i * 4);
                                if (event.y >= yFilm && event.y <= yFilm + 2 && event.x >= xButonFilm && event.x <= xButonFilm + latimeButonFilm) {
                                    indexSelectat = indexReal; locuriSelectateCurent.clear(); stareCurenta = SELECTIE_LOCURI; 
                                }
                            }
                        }
                    }
                    else if (stareCurenta == SELECTIE_LOCURI) {
                        int startY = 8; int yButoane = startY + (programAzi[indexSelectat].getSala().getRanduri() * 2) + 3;
                        if (event.y >= yButoane && event.y <= yButoane + 2 && event.x >= xButon - 16 && event.x <= xButon - 16 + 27) {
                            stareCurenta = BILETE; locuriSelectateCurent.clear(); 
                        }
                        else if (!locuriSelectateCurent.empty() && event.y >= yButoane && event.y <= yButoane + 2 && event.x >= xButon + 16 && event.x <= xButon + 16 + 27) {
                            for (auto loc : locuriSelectateCurent) {
                                programAzi[indexSelectat].rezervaLoc(loc.first, loc.second);
                                float pretBiletDinamice = programAzi[indexSelectat].getPretLoc(loc.first);
                                string biletText = "Bilet: " + programAzi[indexSelectat].getFilm().getTitlu() + " (R" + to_string(loc.first+1) + " L" + to_string(loc.second+1) + ")";
                                cos.push_back(ProdusCos(biletText, pretBiletDinamice));
                            }
                            locuriSelectateCurent.clear(); 
                        }
                        else {
                            int latimeTotalaSalaCaractere = programAzi[indexSelectat].getSala().getLocuriPeRand() * 6;
                            int xSalaCentratDinamice = (COLS - latimeTotalaSalaCaractere) / 2;
                            if (xSalaCentratDinamice < 0) xSalaCentratDinamice = 0;
                            for (int i = 0; i < programAzi[indexSelectat].getSala().getRanduri(); i++) {
                                for (int j = 0; j < programAzi[indexSelectat].getSala().getLocuriPeRand(); j++) {
                                    int posX = xSalaCentratDinamice + (j * 6);
                                    if (event.y == startY + (i * 2) && event.x >= posX && event.x <= posX + 5) {
                                        if (programAzi[indexSelectat].esteLiber(i, j)) {
                                            bool gasit = false;
                                            for(size_t k=0; k < locuriSelectateCurent.size(); k++) {
                                                if(locuriSelectateCurent[k].first == i && locuriSelectateCurent[k].second == j) {
                                                    locuriSelectateCurent.erase(locuriSelectateCurent.begin() + k); gasit = true; break;
                                                }
                                            }
                                            if(!gasit) locuriSelectateCurent.push_back({i, j});
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (stareCurenta == BAR) {
                        int latimeCasuta = 36; int spatiuIntre = 4;
                        int xStartGrid = (COLS - ((latimeCasuta * 2) + spatiuIntre)) / 2;
                        int yControls = 9 + (maxRanduriBar * 4) + 1; int xControls = (COLS - 60) / 2;
                        int maxOffsetBar = ((meniuBar.size() + 1) / 2) - maxRanduriBar;

                        if (event.y >= yControls && event.y <= yControls + 2) {
                            if (offsetBar > 0 && event.x >= xControls && event.x <= xControls + 16) { offsetBar--; }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = MENIU_PRINCIPAL; offsetBar = 0; }
                            else if (offsetBar < maxOffsetBar && event.x >= xControls + 44 && event.x <= xControls + 60) { offsetBar++; }
                        }
                        else {
                            // Preluam ceasul real pentru click
                            time_t t = time(0); tm* acum = localtime(&t);
                            bool esteHappyHourNachos = (acum->tm_wday == 6 && acum->tm_hour >= 14 && acum->tm_hour < 17);

                            for(int r = 0; r < maxRanduriBar; r++) {
                                int randReal = offsetBar + r;
                                 for(int c = 0; c < 2; c++) {
                                    int indexProdus = (randReal * 2) + c;
                                    if(indexProdus >= meniuBar.size()) break;
                                    int xCurent = xStartGrid + (c * (latimeCasuta + spatiuIntre));
                                    int yProdus = 9 + (r * 4);
                                    if (event.y >= yProdus && event.y <= yProdus + 2 && event.x >= xCurent && event.x <= xCurent + latimeCasuta) {
                                        
                                        string numeP = meniuBar[indexProdus].nume;
                                        float pretAdaugat = meniuBar[indexProdus].pret;
                                        
                                        // Aplicam reducerea si la calculul final cand se adauga in cos
                                        if (numeP.find("Nachos") != string::npos && !(numeP.find("Combo") != string::npos)) {
                                            if (esteHappyHourNachos) {
                                                pretAdaugat = pretAdaugat * 0.50;
                                                numeP += " (Promo 50%)";
                                            }
                                        }

                                        char buffer[100];
                                        snprintf(buffer, sizeof(buffer), "%s: %.2f RON", numeP.c_str(), pretAdaugat);
                                        deseneazaButonD(yProdus, xCurent, string(buffer), 5, latimeCasuta); 
                                        refresh(); napms(100); 
                                        
                                        cos.push_back(ProdusCos(numeP, pretAdaugat));
                                    }
                                }
                            }
                        }
                    }
                    else if (stareCurenta == COS) {
                        int yControls = 9 + (maxCosPeEcran * 4); int xControls = (COLS - 60) / 2; 
                        if (event.y >= yControls && event.y <= yControls + 2 && event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = stareAnterioara; }
                        else if (!cos.empty() && event.y >= yControls + 4 && event.y <= yControls + 6 && event.x >= xButon - 16 && event.x <= xButon - 16 + 27) { cos.clear(); offsetCos = 0; }
                        else if (!cos.empty() && event.y >= yControls + 4 && event.y <= yControls + 6 && event.x >= xButon + 16 && event.x <= xButon + 16 + 27) { cos.clear(); stareCurenta = MENIU_PRINCIPAL; }
                        else {
                            for(int i = 0; i < maxCosPeEcran; i++) {
                                int indexReal = offsetCos + i; if(indexReal >= cos.size()) break;
                                if (event.y == 10 + (i * 4) && event.x >= xButonFilm + latimeButonFilm + 2 && event.x <= xButonFilm + latimeButonFilm + 6) {
                                    cos.erase(cos.begin() + indexReal);
                                    if (offsetCos > 0 && offsetCos >= cos.size()) offsetCos--;
                                    break; 
                                }
                            }
                        }
                    }
                    else if (stareCurenta == PROMOTII) {
                        int yControls = 9 + (maxPromotiiPeEcran * 6); int xControls = (COLS - 60) / 2;
                        int maxOffsetPromotii = listaPromotii.size() - maxPromotiiPeEcran;
                        if (event.y >= yControls && event.y <= yControls + 2) {
                            if (offsetPromotii > 0 && event.x >= xControls && event.x <= xControls + 16) { offsetPromotii--; }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = MENIU_PRINCIPAL; offsetPromotii = 0; }
                            else if (offsetPromotii < maxOffsetPromotii && event.x >= xControls + 44 && event.x <= xControls + 60) { offsetPromotii++; }
                        }
                    }
                }
            }
        }
        else if (actiune == '0') {
            if (stareCurenta == MENIU_PRINCIPAL) ruleaza = false;
            else if (stareCurenta == SELECTIE_LOCURI) stareCurenta = BILETE;
            else { stareCurenta = MENIU_PRINCIPAL; offsetFilme = 0; offsetBar = 0; offsetPromotii = 0; offsetCos = 0; }
        }
    }
    endwin();              
    printf("\033?1000l\n"); 
    return 0;
}