#include <ncurses.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio> 
#include "Film.h" 
#include "Sala.h" 
#include "Proiectie.h" 

using namespace std;

enum StareAplicatie { MENIU_PRINCIPAL, BILETE, SELECTIE_LOCURI, BAR, PROMOTII };

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

// --- NOU: Design tip CUPON pentru Promotii (Are 5 randuri inaltime) ---
void deseneazaCasutaPromotie(int y, int x, string titlu, string descriere, int culoare, int latime) {
    attron(COLOR_PAIR(culoare) | A_BOLD);
    
    // Top border
    mvprintw(y, x, "+");
    for(int i = 0; i < latime - 2; i++) printw("-");
    printw("+");

    // Title (Randul 1)
    mvprintw(y + 1, x, "|");
    int padT1 = (latime - 2 - titlu.length()) / 2;
    int padT2 = (latime - 2 - titlu.length()) - padT1;
    for(int i=0; i<padT1; i++) printw(" ");
    printw("%s", titlu.c_str());
    for(int i=0; i<padT2; i++) printw(" ");
    printw("|");

    // Separator (Randul 2) - Linie punctata
    mvprintw(y + 2, x, "|");
    for(int i = 0; i < latime - 2; i++) printw(".");
    printw("|");

    // Description (Randul 3)
    mvprintw(y + 3, x, "|");
    int padD1 = (latime - 2 - descriere.length()) / 2;
    int padD2 = (latime - 2 - descriere.length()) - padD1;
    for(int i=0; i<padD1; i++) printw(" ");
    printw("%s", descriere.c_str());
    for(int i=0; i<padD2; i++) printw(" ");
    printw("|");

    // Bottom border
    mvprintw(y + 4, x, "+");
    for(int i = 0; i < latime - 2; i++) printw("-");
    printw("+");

    attroff(COLOR_PAIR(culoare) | A_BOLD);
}

void deseneazaEcranCinema(int y, int x) {
    attron(COLOR_PAIR(1) | A_BOLD); 
    mvprintw(y, x,     "+----------------------------------------------------------------------------------------------------+");
    mvprintw(y + 1, x, "|                                             E C R A N                                              |");
    mvprintw(y + 2, x, "+----------------------------------------------------------------------------------------------------+");
    attroff(COLOR_PAIR(1) | A_BOLD);
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

    mousemask(BUTTON1_PRESSED | BUTTON1_CLICKED | BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);
    printf("\033[?1000h\n"); 

    Sala salaIMAX("Sala IMAX", 8, 12);
    Sala salaVIP("Sala VIP", 8, 12);
    Sala salaStd1("Sala Standard 1", 8, 12);
    Sala salaStd2("Sala Standard 2", 8, 12);

    Film f1("Deadpool & Wolverine", 127, 30.00);
    Film f2("Dune: Partea II", 166, 35.50);
    Film f3("Oppenheimer", 180, 25.00);
    Film f4("Joker: Folie a Deux", 138, 30.00);

    vector<Proiectie> programAzi;
    programAzi.push_back(Proiectie(f1, salaIMAX, "10:00"));
    programAzi.push_back(Proiectie(f2, salaVIP, "10:15"));
    programAzi.push_back(Proiectie(f3, salaStd1, "10:30"));
    programAzi.push_back(Proiectie(f4, salaStd2, "10:45"));
    programAzi.push_back(Proiectie(f4, salaIMAX, "13:00"));
    programAzi.push_back(Proiectie(f1, salaVIP, "13:30"));
    programAzi.push_back(Proiectie(f2, salaStd1, "14:00"));
    programAzi.push_back(Proiectie(f3, salaStd2, "14:15"));
    programAzi.push_back(Proiectie(f3, salaIMAX, "16:00"));
    programAzi.push_back(Proiectie(f4, salaVIP, "16:30"));
    programAzi.push_back(Proiectie(f1, salaStd1, "17:15"));
    programAzi.push_back(Proiectie(f2, salaStd2, "18:00"));
    programAzi.push_back(Proiectie(f2, salaIMAX, "20:00"));
    programAzi.push_back(Proiectie(f3, salaVIP, "19:30")); 
    programAzi.push_back(Proiectie(f4, salaStd1, "20:30"));
    programAzi.push_back(Proiectie(f1, salaStd2, "21:30"));

    vector<ProdusBar> meniuBar;
    meniuBar.push_back(ProdusBar("Apa Plata 0.5L", 8.00));
    meniuBar.push_back(ProdusBar("Apa Minerala 0.5L", 8.50));
    meniuBar.push_back(ProdusBar("Suc Cola/Fanta 0.5L", 12.00));
    meniuBar.push_back(ProdusBar("Lipton Ice Tea 0.5L", 12.00));
    meniuBar.push_back(ProdusBar("Popcorn Mic", 15.00));
    meniuBar.push_back(ProdusBar("Popcorn Mare", 22.00));
    meniuBar.push_back(ProdusBar("Portie Nachos", 20.00));
    meniuBar.push_back(ProdusBar("Popcorn Mic + Suc", 24.00));
    meniuBar.push_back(ProdusBar("Popcorn Mare + Suc", 30.00));
    meniuBar.push_back(ProdusBar("Nachos + Suc", 29.00));

    // --- BAZA DE DATE PENTRU PROMOTII ---
    vector<Promotie> listaPromotii;
    listaPromotii.push_back(Promotie("MARTEA FILMULUI", "1+1 Gratis la orice bilet cumparat in zilele de marti."));
    listaPromotii.push_back(Promotie("REDUCERE ELEVI/STUDENTI", "Reducere 20% la orice film pe baza carnetului vizat."));
    listaPromotii.push_back(Promotie("PACHETUL FAMILIEI", "4 Bilete + 2 Popcorn Mare + 4 Sucuri = Doar 160 RON!"));
    listaPromotii.push_back(Promotie("HAPPY HOUR LA BAR", "Vineri intre 14:00 - 17:00 primesti 50% reducere la Nachos."));
    listaPromotii.push_back(Promotie("PREMIERE VIP", "La achizitia unui bilet VIP primesti o sampanie gratuit."));

    bool ruleaza = true;
    MEVENT event; 
    StareAplicatie stareCurenta = MENIU_PRINCIPAL;
    int indexSelectat = -1; 
    
    int offsetFilme = 0; 
    int maxFilmePeEcran = 3; 

    int offsetBar = 0; 
    int maxRanduriBar = 3; 

    // Variabile pentru Promotii
    int offsetPromotii = 0;
    int maxPromotiiPeEcran = 2; // Aratam cate 2 cupoane simultan, fiind inalte

    while (ruleaza) {
        int xLogo = (COLS - 49) / 2;
        if (xLogo < 0) xLogo = 0; 
        
        int xButon = (COLS - 28) / 2;
        if (xButon < 0) xButon = 0;

        int latimeSala = 102;
        int xSalaCentrat = (COLS - latimeSala) / 2;
        if (xSalaCentrat < 0) xSalaCentrat = 0;
        
        int latimeButonFilm = 66; 
        int xButonFilm = (COLS - latimeButonFilm) / 2;

        int latimeCupon = 70; // Cupoanele le facem mai late ca sa incapa descrierea
        int xCupon = (COLS - latimeCupon) / 2;

        clear();
        
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

                char buffer[100];
                snprintf(buffer, sizeof(buffer), "[%s] %s | %s - %.2f RON", 
                         programAzi[indexReal].getOra().c_str(),
                         programAzi[indexReal].getFilm().getTitlu().c_str(), 
                         programAzi[indexReal].getSala().getNume().c_str(),
                         programAzi[indexReal].getFilm().getPret());
                
                deseneazaButonD(9 + (i * 4), xButonFilm, string(buffer), 1, latimeButonFilm);
            }
            
            int yControls = 21;
            int xControls = (COLS - 60) / 2; 
            
            if (offsetFilme > 0) { deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16); }
            deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
            if (offsetFilme + maxFilmePeEcran < programAzi.size()) { deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16); }
        }
        else if (stareCurenta == SELECTIE_LOCURI && indexSelectat != -1) {
            deseneazaEcranCinema(2, xSalaCentrat);
            
            attron(COLOR_PAIR(2) | A_BOLD);
            string detalii = "Film: " + programAzi[indexSelectat].getFilm().getTitlu() + 
                             " | Ora: " + programAzi[indexSelectat].getOra() + 
                             " | " + programAzi[indexSelectat].getSala().getNume();
            mvprintw(6, (COLS - detalii.length()) / 2, "%s", detalii.c_str());
            attroff(COLOR_PAIR(2) | A_BOLD);

            int startY = 8; 
            for (int i = 0; i < programAzi[indexSelectat].getSala().getRanduri(); i++) {
                mvprintw(startY + (i * 2), xSalaCentrat - 10, "Rand %d", i + 1); 

                for (int j = 0; j < programAzi[indexSelectat].getSala().getLocuriPeRand(); j++) {
                    int offsetCuloar = (j >= 6) ? 6 : 0; 
                    int posX = xSalaCentrat + (j * 8) + offsetCuloar;

                    if (programAzi[indexSelectat].esteLiber(i, j)) {
                        attron(COLOR_PAIR(3)); 
                        mvprintw(startY + (i * 2), posX, "[ %02d ]", j + 1); 
                        attroff(COLOR_PAIR(3));
                    } else {
                        attron(COLOR_PAIR(4)); 
                        mvprintw(startY + (i * 2), posX, "[ XX ]"); 
                        attroff(COLOR_PAIR(4));
                    }
                }
            }
            deseneazaButonD(startY + (programAzi[indexSelectat].getSala().getRanduri() * 2) + 2, xButon, "0. Inapoi la Filme", 4, 28);
        }
        else if (stareCurenta == BAR) {
            afiseazaAntet(xLogo);
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(7, xLogo + 6, "[ SECTIUNEA BAR - GUSTARI & BAUTURI ]");
            attroff(COLOR_PAIR(3) | A_BOLD);

            int latimeCasuta = 36; 
            int spatiuIntre = 4;
            int latimeTotala = (latimeCasuta * 2) + spatiuIntre; 
            int xStartGrid = (COLS - latimeTotala) / 2;
            if (xStartGrid < 0) xStartGrid = 0;

            for(int r = 0; r < maxRanduriBar; r++) {
                int randReal = offsetBar + r;
                for(int c = 0; c < 2; c++) {
                    int indexProdus = (randReal * 2) + c;
                    if(indexProdus >= meniuBar.size()) break;

                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "%s: %.2f RON", meniuBar[indexProdus].nume.c_str(), meniuBar[indexProdus].pret);
                    int xCurent = xStartGrid + (c * (latimeCasuta + spatiuIntre));
                    deseneazaButonD(9 + (r * 4), xCurent, string(buffer), 2, latimeCasuta); 
                }
            }

            int yControls = 9 + (maxRanduriBar * 4) + 1; 
            int xControls = (COLS - 60) / 2; 
            int totalRanduriNevoiase = (meniuBar.size() + 1) / 2;
            int maxOffsetBar = totalRanduriNevoiase - maxRanduriBar;
            if (maxOffsetBar < 0) maxOffsetBar = 0;

            if (offsetBar > 0) { deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16); }
            deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
            if (offsetBar < maxOffsetBar) { deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16); }
        }
        else if (stareCurenta == PROMOTII) {
            afiseazaAntet(xLogo);
            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(7, xLogo + 5, "[ SECTIUNEA OFERTE SI PROMOTII ]");
            attroff(COLOR_PAIR(3) | A_BOLD);

            // --- LOGICA PENTRU DESENARE CUPOANE ---
            for(int i = 0; i < maxPromotiiPeEcran; i++) {
                int indexReal = offsetPromotii + i;
                if(indexReal >= listaPromotii.size()) break; 

                // Alternam culorile cupoanelor: Unul Cyan(1), urmatorul Galben(2) pentru aspect premium
                int culoareCupon = (i % 2 == 0) ? 1 : 2; 
                
                // Un cupon are 5 rânduri (y, y+1...y+4), deci lăsăm un spațiu de 6 rânduri între ele
                deseneazaCasutaPromotie(9 + (i * 6), xCupon, 
                                        listaPromotii[indexReal].titlu, 
                                        listaPromotii[indexReal].descriere, 
                                        culoareCupon, latimeCupon);
            }

            // --- CONTROALE SCROLL PROMOTII ---
            int yControls = 9 + (maxPromotiiPeEcran * 6); 
            int xControls = (COLS - 60) / 2; 

            if (offsetPromotii > 0) { deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16); }
            deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
            if (offsetPromotii + maxPromotiiPeEcran < listaPromotii.size()) { deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16); }
        }
        
        refresh(); 

        int actiune = getch();
        if (actiune == KEY_RESIZE) continue; 
        
        // --- LOGICA SAGETI TASTATURA ---
        if (stareCurenta == BILETE) {
            if (actiune == KEY_UP && offsetFilme > 0) { offsetFilme--; continue; }
            if (actiune == KEY_DOWN && offsetFilme + maxFilmePeEcran < programAzi.size()) { offsetFilme++; continue; }
        }
        else if (stareCurenta == BAR) {
            int totalRanduriNevoiase = (meniuBar.size() + 1) / 2;
            int maxOffsetBar = totalRanduriNevoiase - maxRanduriBar;
            if (maxOffsetBar < 0) maxOffsetBar = 0;
            if (actiune == KEY_UP && offsetBar > 0) { offsetBar--; continue; }
            if (actiune == KEY_DOWN && offsetBar < maxOffsetBar) { offsetBar++; continue; }
        }
        else if (stareCurenta == PROMOTII) {
            if (actiune == KEY_UP && offsetPromotii > 0) { offsetPromotii--; continue; }
            if (actiune == KEY_DOWN && offsetPromotii + maxPromotiiPeEcran < listaPromotii.size()) { offsetPromotii++; continue; }
        }

        // --- LOGICA MOUSE ---
        if (actiune == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                
                // MOUSE WHEEL
                if (stareCurenta == BILETE) {
                    if ((event.bstate & BUTTON4_PRESSED) && offsetFilme > 0) { offsetFilme--; continue; }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetFilme + maxFilmePeEcran < programAzi.size()) { offsetFilme++; continue; }
                }
                else if (stareCurenta == BAR) {
                    int totalRanduriNevoiase = (meniuBar.size() + 1) / 2;
                    int maxOffsetBar = totalRanduriNevoiase - maxRanduriBar;
                    if (maxOffsetBar < 0) maxOffsetBar = 0;
                    if ((event.bstate & BUTTON4_PRESSED) && offsetBar > 0) { offsetBar--; continue; }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetBar < maxOffsetBar) { offsetBar++; continue; }
                }
                else if (stareCurenta == PROMOTII) {
                    if ((event.bstate & BUTTON4_PRESSED) && offsetPromotii > 0) { offsetPromotii--; continue; }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetPromotii + maxPromotiiPeEcran < listaPromotii.size()) { offsetPromotii++; continue; }
                }

                // CLICK NORMAL
                if (event.bstate & BUTTON1_PRESSED || event.bstate & BUTTON1_CLICKED) {
                    
                    if (stareCurenta == MENIU_PRINCIPAL) {
                        if (event.x >= xButon && event.x <= xButon + 27) {
                            if (event.y >= 7 && event.y <= 9) stareCurenta = BILETE;
                            else if (event.y >= 11 && event.y <= 13) stareCurenta = BAR;
                            else if (event.y >= 15 && event.y <= 17) stareCurenta = PROMOTII;
                            else if (event.y >= 19 && event.y <= 21) ruleaza = false;
                        }
                    }
                    else if (stareCurenta == BILETE) {
                        int yControls = 21;
                        int xControls = (COLS - 60) / 2;
                        
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
                                    indexSelectat = indexReal;
                                    stareCurenta = SELECTIE_LOCURI; 
                                }
                            }
                        }
                    }
                    else if (stareCurenta == SELECTIE_LOCURI) {
                        int yButonInapoi = 8 + (programAzi[indexSelectat].getSala().getRanduri() * 2) + 2;
                        if (event.y >= yButonInapoi && event.y <= yButonInapoi + 2 && event.x >= xButon && event.x <= xButon + 27) {
                            stareCurenta = BILETE; 
                        }
                        else {
                            int startY = 8;
                            for (int i = 0; i < programAzi[indexSelectat].getSala().getRanduri(); i++) {
                                for (int j = 0; j < programAzi[indexSelectat].getSala().getLocuriPeRand(); j++) {
                                    int offsetCuloar = (j >= 6) ? 6 : 0;
                                    int posX = xSalaCentrat + (j * 8) + offsetCuloar;
                                    
                                    if (event.y == startY + (i * 2) && event.x >= posX && event.x <= posX + 5) {
                                        programAzi[indexSelectat].rezervaLoc(i, j); 
                                    }
                                }
                            }
                        }
                    }
                    else if (stareCurenta == BAR) {
                        int yControls = 9 + (maxRanduriBar * 4) + 1;
                        int xControls = (COLS - 60) / 2;
                        int totalRanduriNevoiase = (meniuBar.size() + 1) / 2;
                        int maxOffsetBar = totalRanduriNevoiase - maxRanduriBar;
                        if (maxOffsetBar < 0) maxOffsetBar = 0;

                        if (event.y >= yControls && event.y <= yControls + 2) {
                            if (offsetBar > 0 && event.x >= xControls && event.x <= xControls + 16) { offsetBar--; }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = MENIU_PRINCIPAL; offsetBar = 0; }
                            else if (offsetBar < maxOffsetBar && event.x >= xControls + 44 && event.x <= xControls + 60) { offsetBar++; }
                        }
                    }
                    else if (stareCurenta == PROMOTII) {
                        int yControls = 9 + (maxPromotiiPeEcran * 6);
                        int xControls = (COLS - 60) / 2;

                        if (event.y >= yControls && event.y <= yControls + 2) {
                            if (offsetPromotii > 0 && event.x >= xControls && event.x <= xControls + 16) { offsetPromotii--; }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = MENIU_PRINCIPAL; offsetPromotii = 0; }
                            else if (offsetPromotii + maxPromotiiPeEcran < listaPromotii.size() && event.x >= xControls + 44 && event.x <= xControls + 60) { offsetPromotii++; }
                        }
                    }
                }
            }
        }
        else if (actiune == '0') {
            if (stareCurenta == MENIU_PRINCIPAL) ruleaza = false;
            else if (stareCurenta == SELECTIE_LOCURI) stareCurenta = BILETE;
            else { stareCurenta = MENIU_PRINCIPAL; offsetFilme = 0; offsetBar = 0; offsetPromotii = 0; }
        }
    }

    endwin();              
    printf("\033[?1000l\n"); 
    return 0;
}