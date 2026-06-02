#include <ncurses.h>
#include <ctime>
#include "Gestiune.h"
#include "Interfata.h"
#include "BazaDate.h"   
#include "Ecrane.h"     

using namespace std;

enum StareAplicatie { MENIU_PRINCIPAL, BILETE, SELECTIE_LOCURI, BAR, PROMOTII, COS, ADMIN_LOGIN, ADMIN_MENIU, ADMIN_BILETE, ADMIN_BAR, AVERTISMENT_INTARZIERE };

int main() {
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); curs_set(0);          
    start_color(); use_default_colors(); 
    init_pair(1, COLOR_CYAN, -1); init_pair(2, COLOR_YELLOW, -1); init_pair(3, COLOR_GREEN, -1);    
    init_pair(4, COLOR_RED, -1);  init_pair(5, COLOR_WHITE, -1);  init_pair(6, COLOR_MAGENTA, -1);  
    mousemask(BUTTON1_PRESSED | BUTTON1_CLICKED | BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);
    printf("\033[?1000h\n"); 

    time_t t_prog = time(0); tm* acum_prog = localtime(&t_prog);
    
    // Incarcam bazele de date prin noul modul curat
    vector<Sala> complexCinema = BazaDate::incarcaSali();
    vector<Film> catalogFilme = BazaDate::incarcaFilme();
    vector<Proiectie> programAzi = BazaDate::incarcaProgram(acum_prog->tm_wday, catalogFilme, complexCinema);
    BazaDate::incarcaRezervari(programAzi);
    vector<ProdusBar> meniuBar = BazaDate::incarcaBar();
    vector<Promotie> listaPromotii = BazaDate::incarcaPromotii();

    bool ruleaza = true;
    MEVENT event; 
    StareAplicatie stareCurenta = MENIU_PRINCIPAL, stareAnterioara = MENIU_PRINCIPAL; 
    int indexSelectat = -1, indexInAsteptare = -1; 
    int offsetFilme = 0, offsetBar = 0, offsetPromotii = 0, offsetCos = 0, offsetAdmin = 0; 
    int maxFilmePeEcran = 3, maxRanduriBar = 3, maxPromotiiPeEcran = 2, maxCosPeEcran = 5, maxAdminPeEcran = 6; 

    vector<ProdusCos> cos;
    vector<pair<int, int>> locuriSelectateCurent;
    vector<string> istoricVanzari; 
    string parolaIntrodusa = ""; bool eroareParola = false;

    while (ruleaza) {
        time_t t_now = time(0); tm* acum_now = localtime(&t_now);
        int timpCurentMin = acum_now->tm_hour * 60 + acum_now->tm_min;

        for (auto it = programAzi.begin(); it != programAzi.end(); ) {
            int hFilm = atoi(it->getOra().substr(0, 2).c_str()); int mFilm = atoi(it->getOra().substr(3, 2).c_str());
            if (timpCurentMin - (hFilm * 60 + mFilm) > 30) it = programAzi.erase(it); else ++it;
        }
        if (offsetFilme >= programAzi.size() && offsetFilme > 0) offsetFilme = 0;

        int xLogo = (COLS - 49) / 2; if (xLogo < 0) xLogo = 0; 
        int xButon = (COLS - 28) / 2; if (xButon < 0) xButon = 0;
        int latimeButonFilm = 74; int xButonFilm = (COLS - latimeButonFilm) / 2;

        clear();
        
        if (stareCurenta != COS) {
            float totalCos = 0.0; for(auto p : cos) totalCos += p.pret; char bufferCos[50];
            snprintf(bufferCos, sizeof(bufferCos), "COS [%d] - %.2f RON", (int)cos.size(), totalCos);
            if (COLS - 32 > 0) deseneazaButonD(1, COLS - 32, string(bufferCos), 2, 30);
        }

        if (stareCurenta == MENIU_PRINCIPAL) deseneazaButonD(1, 2, "ADMINISTRATOR", 4, 19);

        // Randarea separata a ecranelor
        switch(stareCurenta) {
            case MENIU_PRINCIPAL: Ecrane::afiseazaMeniuPrincipal(xLogo, xButon); break;
            case ADMIN_LOGIN: Ecrane::afiseazaAdminLogin(xLogo, xButon, parolaIntrodusa, eroareParola); break;
            case ADMIN_MENIU: Ecrane::afiseazaAdminMeniu(xLogo, xButon); break;
            case BILETE: Ecrane::afiseazaBilete(xLogo, xButonFilm, latimeButonFilm, programAzi, offsetFilme, maxFilmePeEcran, listaPromotii, acum_now, timpCurentMin); break;
            case SELECTIE_LOCURI: if (indexSelectat != -1) Ecrane::afiseazaSala(indexSelectat, programAzi[indexSelectat], locuriSelectateCurent, listaPromotii, acum_now, xButon); break;
            case BAR: {
                afiseazaAntet(xLogo);
                attron(COLOR_PAIR(3) | A_BOLD); mvprintw(7, xLogo + 6, "[ SECTIUNEA BAR - GUSTARI & BAUTURI ]"); attroff(COLOR_PAIR(3) | A_BOLD);
                int latimeCasuta = 36; int spatiuIntre = 4; int xStartGrid = (COLS - ((latimeCasuta * 2) + spatiuIntre)) / 2;
                for(int r = 0; r < maxRanduriBar; r++) {
                    int randReal = offsetBar + r;
                    for(int c = 0; c < 2; c++) {
                        int indexProdus = (randReal * 2) + c; if(indexProdus >= meniuBar.size()) break;
                        int culoareProdus = 1; string tagPromotie = "";
                        float pretFinalProdus = obtinePretBarDinamic(meniuBar[indexProdus].nume, meniuBar[indexProdus].pret, listaPromotii, acum_now->tm_wday, acum_now->tm_hour, acum_now->tm_min, tagPromotie);
                        string numeP = meniuBar[indexProdus].nume + tagPromotie;
                        if (numeP.find("Combo") != string::npos) culoareProdus = 2; else if (numeP.find("Popcorn") != string::npos || numeP.find("Nachos") != string::npos) culoareProdus = 3;
                        char buffer[100]; snprintf(buffer, sizeof(buffer), "%s: %.2f RON", numeP.c_str(), pretFinalProdus);
                        int xCurent = xStartGrid + (c * (latimeCasuta + spatiuIntre));
                        deseneazaButonD(9 + (r * 4), xCurent, string(buffer), culoareProdus, latimeCasuta); 
                    }
                }
                int yControls = 9 + (maxRanduriBar * 4) + 1; int xControls = (COLS - 60) / 2; int maxOffsetBar = ((meniuBar.size() + 1) / 2) - maxRanduriBar;
                if (offsetBar > 0) deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16);
                deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
                if (offsetBar < maxOffsetBar) deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16);
                break;
            }
            case PROMOTII: {
                afiseazaAntet(xLogo);
                attron(COLOR_PAIR(3) | A_BOLD); mvprintw(7, xLogo + 5, "[ SECTIUNEA OFERTE SI PROMOTII ]"); attroff(COLOR_PAIR(3) | A_BOLD);
                int latimeCupon = 70; int xCupon = (COLS - latimeCupon) / 2;
                for(int i = 0; i < maxPromotiiPeEcran; i++) {
                    int indexReal = offsetPromotii + i; if(indexReal >= listaPromotii.size()) break; 
                    int culoareCupon = (i % 2 == 0) ? 1 : 2; 
                    deseneazaCasutaPromotie(9 + (i * 6), xCupon, listaPromotii[indexReal].titlu, listaPromotii[indexReal].descriere, culoareCupon, latimeCupon);
                }
                int yControls = 9 + (maxPromotiiPeEcran * 6); int xControls = (COLS - 60) / 2; 
                if (offsetPromotii > 0) deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16);
                deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
                if (offsetPromotii + maxPromotiiPeEcran < listaPromotii.size()) deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16);
                break;
            }
            case COS: {
                afiseazaAntet(xLogo);
                attron(COLOR_PAIR(2) | A_BOLD); mvprintw(7, xLogo + 9, "[ COSUL TAU DE CUMPARATURI ]"); attroff(COLOR_PAIR(2) | A_BOLD);
                if (cos.empty()) mvprintw(11, xLogo + 15, "Cosul tau este gol!"); 
                else {
                    for(int i = 0; i < maxCosPeEcran; i++) {
                        int indexReal = offsetCos + i; if(indexReal >= cos.size()) break; 
                        char buffer[120]; snprintf(buffer, sizeof(buffer), "%d. %-35s | %.2f RON", indexReal + 1, cos[indexReal].nume.c_str(), cos[indexReal].pret);
                        deseneazaButonD(9 + (i * 4), xButonFilm, string(buffer), 1, latimeButonFilm);
                        attron(COLOR_PAIR(4) | A_BOLD); mvprintw(10 + (i * 4), xButonFilm + latimeButonFilm + 2, "[ X ]"); attroff(COLOR_PAIR(4) | A_BOLD);
                    }
                }
                int yControls = 9 + (maxCosPeEcran * 4); int xControls = (COLS - 60) / 2; int maxOffsetCos = cos.size() - maxCosPeEcran; if (maxOffsetCos < 0) maxOffsetCos = 0;
                if (offsetCos > 0) deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16);
                deseneazaButonD(yControls, xControls + 20, "<- Inapoi", 4, 20);
                if (offsetCos < maxOffsetCos) deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16);
                if (!cos.empty()) {
                    deseneazaButonD(yControls + 4, xButon - 16, "Sterge Tot", 4, 28);
                    deseneazaButonD(yControls + 4, xButon + 16, "Finalizeaza Comanda", 3, 28);
                }
                break;
            }
            case ADMIN_BILETE:
            case ADMIN_BAR: {
                afiseazaAntet(xLogo);
                attron(COLOR_PAIR(4) | A_BOLD);
                if (stareCurenta == ADMIN_BILETE) mvprintw(7, xLogo + 5, "[ TABEL VANZARI BILETE CUMPARATE ]");
                else mvprintw(7, xLogo + 5, "[ TABEL VANZARI BAUTURI & SNACKS ]");
                attroff(COLOR_PAIR(4) | A_BOLD);
                int startY = 9;
                attron(COLOR_PAIR(2));
                if (stareCurenta == ADMIN_BILETE) mvprintw(startY, 4, "FILM                         | ORA     | SALA         | DATA         | BILETE VANDUTE | INCASARI TOTALE");
                else mvprintw(startY, 4, "PRODUS BAR SAU PROMOTIE           | DATA       | ORA   | CANTIT.  | INCASARI TOTALE");
                mvprintw(startY+1, 4, "--------------------------------------------------------------------------------------------------------");
                attroff(COLOR_PAIR(2));
                for(int i = 0; i < maxAdminPeEcran; i++) {
                    int indexReal = offsetAdmin + i; if(indexReal >= istoricVanzari.size()) break;
                    mvprintw(startY + 3 + (i * 2), 4, "%d. %s", indexReal + 1, istoricVanzari[indexReal].c_str());
                }
                int yControls = startY + 3 + (maxAdminPeEcran * 2); int xControls = (COLS - 60) / 2; int maxOffsetAdmin = istoricVanzari.size() - maxAdminPeEcran; if (maxOffsetAdmin < 0) maxOffsetAdmin = 0;
                if (offsetAdmin > 0) deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16);
                deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
                if (offsetAdmin < maxOffsetAdmin) deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16);
                break;
            }
            case AVERTISMENT_INTARZIERE: {
                if (indexInAsteptare != -1) {
                    afiseazaAntet(xLogo);
                    attron(COLOR_PAIR(4) | A_BOLD); mvprintw(8, (COLS - 33) / 2, "!!! AVERTISMENT INTARZIERE !!!"); attroff(COLOR_PAIR(4) | A_BOLD);
                    string mesaj = "Filmul " + programAzi[indexInAsteptare].getFilm().getTitlu() + " a inceput deja de la " + programAzi[indexInAsteptare].getOra() + "!";
                    mvprintw(10, (COLS - mesaj.length()) / 2, "%s", mesaj.c_str());
                    mvprintw(11, (COLS - 50) / 2, "Sunteti sigur ca doriti sa achizitionati bilete?");
                    deseneazaButonD(14, xButon - 5, "DA, CONTINUA", 3, 18);
                    deseneazaButonD(14, xButon + 15, "IESI", 4, 18);
                }
                break;
            }
        }
        refresh(); 

        int actiune = getch();
        if (actiune == KEY_RESIZE) continue; 
        
        if (stareCurenta == ADMIN_LOGIN) {
            if (actiune == '\n' || actiune == 10 || actiune == KEY_ENTER) {
                if (parolaIntrodusa == BazaDate::verificaParola()) { stareCurenta = ADMIN_MENIU; parolaIntrodusa = ""; } 
                else { eroareParola = true; parolaIntrodusa = ""; }
                continue;
            } else if (actiune == KEY_BACKSPACE || actiune == 127 || actiune == '\b') {
                if (!parolaIntrodusa.empty()) parolaIntrodusa.pop_back(); continue;
            } else if (actiune >= 32 && actiune <= 126 && actiune != KEY_MOUSE && actiune != KEY_RESIZE) {
                if (parolaIntrodusa.length() < 18) { parolaIntrodusa += (char)actiune; } continue;
            }
        }

        if (actiune == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                // --- SCROLL DIN ROTITA MOUSE-ULUI ---
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
                else if (stareCurenta == PROMOTII) {
                    int maxOffsetPromotii = listaPromotii.size() - maxPromotiiPeEcran;
                    if ((event.bstate & BUTTON4_PRESSED) && offsetPromotii > 0) { offsetPromotii--; continue; }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetPromotii < maxOffsetPromotii) { offsetPromotii++; continue; }
                }
                else if (stareCurenta == ADMIN_BILETE || stareCurenta == ADMIN_BAR) { 
                    int maxOffsetAdmin = istoricVanzari.size() - maxAdminPeEcran;
                    if ((event.bstate & BUTTON4_PRESSED) && offsetAdmin > 0) { offsetAdmin--; continue; }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetAdmin < maxOffsetAdmin) { offsetAdmin++; continue; }
                }

                // --- CLICK STÂNGA ---
                if (event.bstate & BUTTON1_PRESSED || event.bstate & BUTTON1_CLICKED) {
                    
                    if (stareCurenta != COS) {
                        int xCosBtn = COLS - 32;
                        if (event.y >= 1 && event.y <= 3 && event.x >= xCosBtn && event.x <= xCosBtn + 30) {
                            stareAnterioara = stareCurenta; stareCurenta = COS; continue;
                        }
                    }

                    if (stareCurenta == MENIU_PRINCIPAL) {
                        if (event.y >= 1 && event.y <= 3 && event.x >= 2 && event.x <= 20) {
                            parolaIntrodusa = ""; eroareParola = false; stareCurenta = ADMIN_LOGIN; continue;
                        }
                        if (event.x >= xButon && event.x <= xButon + 27) {
                            if (event.y >= 7 && event.y <= 9) stareCurenta = BILETE;
                            else if (event.y >= 11 && event.y <= 13) stareCurenta = BAR;
                            else if (event.y >= 15 && event.y <= 17) stareCurenta = PROMOTII;
                            else if (event.y >= 19 && event.y <= 21) ruleaza = false;
                        }
                    }
                    else if (stareCurenta == ADMIN_LOGIN) {
                        if (event.y >= 16 && event.y <= 18) {
                            if (event.x >= xButon - 5 && event.x <= xButon - 5 + 18) { 
                                if (parolaIntrodusa == BazaDate::verificaParola()) { stareCurenta = ADMIN_MENIU; parolaIntrodusa = ""; } 
                                else { eroareParola = true; parolaIntrodusa = ""; }
                            } 
                            else if (event.x >= xButon + 15 && event.x <= xButon + 15 + 18) { stareCurenta = MENIU_PRINCIPAL; }
                        }
                    }
                    else if (stareCurenta == ADMIN_MENIU) { 
                        if (event.x >= xButon && event.x <= xButon + 27) {
                            if (event.y >= 11 && event.y <= 13) { istoricVanzari = BazaDate::genereazaIstoricBilete(); offsetAdmin = 0; stareCurenta = ADMIN_BILETE; }
                            else if (event.y >= 15 && event.y <= 17) { istoricVanzari = BazaDate::genereazaIstoricBar(); offsetAdmin = 0; stareCurenta = ADMIN_BAR; }
                            else if (event.y >= 19 && event.y <= 21) { stareCurenta = MENIU_PRINCIPAL; }
                        }
                    }
                    else if (stareCurenta == ADMIN_BILETE || stareCurenta == ADMIN_BAR) { 
                        int yControls = 9 + 3 + (maxAdminPeEcran * 2); int xControls = (COLS - 60) / 2;
                        int maxOffsetAdmin = istoricVanzari.size() - maxAdminPeEcran;
                        if (event.y >= yControls && event.y <= yControls + 2) {
                            if (offsetAdmin > 0 && event.x >= xControls && event.x <= xControls + 16) { offsetAdmin--; }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = ADMIN_MENIU; offsetAdmin = 0; }
                            else if (offsetAdmin < maxOffsetAdmin && event.x >= xControls + 44 && event.x <= xControls + 60) { offsetAdmin++; }
                        }
                    }
                    else if (stareCurenta == AVERTISMENT_INTARZIERE) {
                        if (event.y >= 14 && event.y <= 16) {
                            if (event.x >= xButon - 5 && event.x <= xButon - 5 + 18) { indexSelectat = indexInAsteptare; locuriSelectateCurent.clear(); stareCurenta = SELECTIE_LOCURI; } 
                            else if (event.x >= xButon + 15 && event.x <= xButon + 15 + 18) { stareCurenta = BILETE; indexInAsteptare = -1; }
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
                                int indexReal = offsetFilme + i; if(indexReal >= programAzi.size()) break;
                                int yFilm = 9 + (i * 4);
                                if (event.y >= yFilm && event.y <= yFilm + 2 && event.x >= xButonFilm && event.x <= xButonFilm + latimeButonFilm) {
                                    int hFilmC = atoi(programAzi[indexReal].getOra().substr(0, 2).c_str());
                                    int mFilmC = atoi(programAzi[indexReal].getOra().substr(3, 2).c_str());
                                    int timpFilmMinC = hFilmC * 60 + mFilmC;
                                    
                                    time_t t_now = time(0); tm* acum_now = localtime(&t_now);
                                    int timpCurentMin = acum_now->tm_hour * 60 + acum_now->tm_min;

                                    if (timpCurentMin - timpFilmMinC >= 0 && timpCurentMin - timpFilmMinC <= 30) { indexInAsteptare = indexReal; stareCurenta = AVERTISMENT_INTARZIERE; } 
                                    else { indexSelectat = indexReal; locuriSelectateCurent.clear(); stareCurenta = SELECTIE_LOCURI; }
                                }
                            }
                        }
                    }
                    else if (stareCurenta == SELECTIE_LOCURI) {
                        int startY = 8; int yButoane = startY + (programAzi[indexSelectat].getSala().getRanduri() * 2) + 3;
                        if (event.y >= yButoane && event.y <= yButoane + 2 && event.x >= xButon - 16 && event.x <= xButon - 16 + 27) { stareCurenta = BILETE; locuriSelectateCurent.clear(); }
                        else if (!locuriSelectateCurent.empty() && event.y >= yButoane && event.y <= yButoane + 2 && event.x >= xButon + 16 && event.x <= xButon + 16 + 27) {
                            for (auto loc : locuriSelectateCurent) {
                                programAzi[indexSelectat].rezervaLoc(loc.first, loc.second);
                                
                                time_t t_now = time(0); tm* acum_now = localtime(&t_now);
                                float pretBiletDinamice = obtinePretBiletDinamic(programAzi[indexSelectat], loc.first, listaPromotii, acum_now->tm_wday, acum_now->tm_hour, acum_now->tm_min, acum_now->tm_mday);
                                string titluFilmM = programAzi[indexSelectat].getFilm().getTitlu();
                                string biletText = "Bilet: " + titluFilmM + " (R" + to_string(loc.first+1) + " L" + to_string(loc.second+1) + ")";
                                
                                char dataStr[15]; strftime(dataStr, sizeof(dataStr), "%d/%m/%Y", acum_now);
                                char bufFisier[200];
                                snprintf(bufFisier, sizeof(bufFisier), "%-25.25s | %-5s | %-12.12s | %-10.10s | %-10s", titluFilmM.c_str(), programAzi[indexSelectat].getOra().c_str(), programAzi[indexSelectat].getSala().getId().c_str(), programAzi[indexSelectat].getFilm().getGen().c_str(), dataStr);
                                cos.push_back(ProdusCos(0, biletText, string(bufFisier), pretBiletDinamice, indexSelectat, loc.first, loc.second));
                            }
                            locuriSelectateCurent.clear(); 
                        }
                        else {
                            int latimeTotalaSalaCaractere = programAzi[indexSelectat].getSala().getLocuriPeRand() * 6; int xSalaCentratDinamice = (COLS - latimeTotalaSalaCaractere) / 2; if (xSalaCentratDinamice < 0) xSalaCentratDinamice = 0;
                            for (int i = 0; i < programAzi[indexSelectat].getSala().getRanduri(); i++) {
                                for (int j = 0; j < programAzi[indexSelectat].getSala().getLocuriPeRand(); j++) {
                                    int posX = xSalaCentratDinamice + (j * 6);
                                    if (event.y == startY + (i * 2) && event.x >= posX && event.x <= posX + 5) {
                                        if (programAzi[indexSelectat].esteLiber(i, j)) {
                                            bool gasit = false;
                                            for(size_t k=0; k < locuriSelectateCurent.size(); k++) {
                                                if(locuriSelectateCurent[k].first == i && locuriSelectateCurent[k].second == j) { locuriSelectateCurent.erase(locuriSelectateCurent.begin() + k); gasit = true; break; }
                                            }
                                            if(!gasit) locuriSelectateCurent.push_back({i, j});
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (stareCurenta == BAR) {
                        int latimeCasuta = 36; int spatiuIntre = 4; int xStartGrid = (COLS - ((latimeCasuta * 2) + spatiuIntre)) / 2;
                        int yControls = 9 + (maxRanduriBar * 4) + 1; int xControls = (COLS - 60) / 2;
                        int maxOffsetBar = ((meniuBar.size() + 1) / 2) - maxRanduriBar;

                        if (event.y >= yControls && event.y <= yControls + 2) {
                            if (offsetBar > 0 && event.x >= xControls && event.x <= xControls + 16) { offsetBar--; }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = MENIU_PRINCIPAL; offsetBar = 0; }
                            else if (offsetBar < maxOffsetBar && event.x >= xControls + 44 && event.x <= xControls + 60) { offsetBar++; }
                        }
                        else {
                            for(int r = 0; r < maxRanduriBar; r++) {
                                int randReal = offsetBar + r; for(int c = 0; c < 2; c++) {
                                    int indexProdus = (randReal * 2) + c; if(indexProdus >= meniuBar.size()) break;
                                    int xCurent = xStartGrid + (c * (latimeCasuta + spatiuIntre)); int yProdus = 9 + (r * 4);
                                    if (event.y >= yProdus && event.y <= yProdus + 2 && event.x >= xCurent && event.x <= xCurent + latimeCasuta) {
                                        
                                        time_t t_now = time(0); tm* acum_now = localtime(&t_now);
                                        string tagPromotie = "";
                                        float pretAdaugat = obtinePretBarDinamic(meniuBar[indexProdus].nume, meniuBar[indexProdus].pret, listaPromotii, acum_now->tm_wday, acum_now->tm_hour, acum_now->tm_min, tagPromotie);
                                        string numeP = meniuBar[indexProdus].nume + tagPromotie;

                                        char dataStr[15]; strftime(dataStr, sizeof(dataStr), "%d/%m/%Y", acum_now);
                                        char oraStr[10]; strftime(oraStr, sizeof(oraStr), "%H:%M", acum_now);
                                        char bufFisierBar[200]; 
                                        snprintf(bufFisierBar, sizeof(bufFisierBar), "%s|%s|%s", numeP.c_str(), dataStr, oraStr);
                                        char buffer[100]; snprintf(buffer, sizeof(buffer), "%s: %.2f RON", numeP.c_str(), pretAdaugat);
                                        deseneazaButonD(yProdus, xCurent, string(buffer), 5, latimeCasuta); refresh(); napms(100); 
                                        cos.push_back(ProdusCos(1, numeP, string(bufFisierBar), pretAdaugat));
                                    }
                                }
                            }
                        }
                    }
                    else if (stareCurenta == COS) {
                        int yControls = 9 + (maxCosPeEcran * 4); int xControls = (COLS - 60) / 2; 
                        if (event.y >= yControls && event.y <= yControls + 2 && event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = stareAnterioara; }
                        else if (!cos.empty() && event.y >= yControls + 4 && event.y <= yControls + 6 && event.x >= xButon - 16 && event.x <= xButon - 16 + 27) { 
                            for (auto& p : cos) { if (p.tip == 0 && p.indexProiectie != -1) { programAzi[p.indexProiectie].elibereazaLoc(p.randScaun, p.locScaun); } }
                            cos.clear(); offsetCos = 0; 
                        }
                        else if (!cos.empty() && event.y >= yControls + 4 && event.y <= yControls + 6 && event.x >= xButon + 16 && event.x <= xButon + 16 + 27) { 
                            BazaDate::finalizeazaComanda(cos, programAzi); 
                            cos.clear(); stareCurenta = MENIU_PRINCIPAL; 
                        }
                        else {
                            for(int i = 0; i < maxCosPeEcran; i++) {
                                int indexReal = offsetCos + i; if(indexReal >= cos.size()) break;
                                if (event.y == 10 + (i * 4) && event.x >= xButonFilm + latimeButonFilm + 2 && event.x <= xButonFilm + latimeButonFilm + 6) {
                                    if (cos[indexReal].tip == 0 && cos[indexReal].indexProiectie != -1) { programAzi[cos[indexReal].indexProiectie].elibereazaLoc(cos[indexReal].randScaun, cos[indexReal].locScaun); }
                                    cos.erase(cos.begin() + indexReal); if (offsetCos > 0 && offsetCos >= cos.size()) offsetCos--; break; 
                                }
                            }
                        }
                    }
                    else if (stareCurenta == PROMOTII) {
                        int yControls = 9 + (maxPromotiiPeEcran * 6); int xControls = (COLS - 60) / 2; int maxOffsetPromotii = listaPromotii.size() - maxPromotiiPeEcran;
                        if (event.y >= yControls && event.y <= yControls + 2) {
                            if (offsetPromotii > 0 && event.x >= xControls && event.x <= xControls + 16) { offsetPromotii--; }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) { stareCurenta = MENIU_PRINCIPAL; offsetPromotii = 0; }
                            else if (offsetPromotii < maxOffsetPromotii && event.x >= xControls + 44 && event.x <= xControls + 60) { offsetPromotii++; }
                        }
                    }
                }
            }
        }
        else if (actiune == '0' && stareCurenta != ADMIN_LOGIN) {
            if (stareCurenta == MENIU_PRINCIPAL) ruleaza = false;
            else if (stareCurenta == SELECTIE_LOCURI) stareCurenta = BILETE;
            else if (stareCurenta == ADMIN_BILETE || stareCurenta == ADMIN_BAR) stareCurenta = ADMIN_MENIU; 
            else if (stareCurenta == AVERTISMENT_INTARZIERE) { stareCurenta = BILETE; indexInAsteptare = -1; } 
            else { stareCurenta = MENIU_PRINCIPAL; offsetFilme = 0; offsetBar = 0; offsetPromotii = 0; offsetCos = 0; offsetAdmin = 0; }
        }
    }
    endwin();              
    printf("\033?1000l\n"); 
    return 0;
}