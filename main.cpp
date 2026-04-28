#include <ncurses.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio> 
#include <locale.h> 
#include "Film.h" 
#include "Sala.h" 
#include "Proiectie.h" 

using namespace std;

enum StareAplicatie { MENIU_PRINCIPAL, BILETE, SELECTIE_LOCURI, BAR, PROMOTII };

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
    
    // Desenam simbolurile premium pe laterale!
    // Le punem la randul 2 (pe mijlocul logoului)
    if (startX > 6 && (startX + 49 + 6) < COLS) {
        mvprintw(2, startX - 4, "🎭");
        
        attron(COLOR_PAIR(2) | A_BOLD); // O facem aurie pe Fleur-de-lis
        mvprintw(2, startX + 51, "⚜︎");
        attroff(COLOR_PAIR(2) | A_BOLD);
    }
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

void deseneazaEcranCinema(int y, int x) {
    attron(COLOR_PAIR(1) | A_BOLD); 
    mvprintw(y, x,     "+----------------------------------------------------------------------------------------------------+");
    mvprintw(y + 1, x, "|                                             E C R A N                                              |");
    mvprintw(y + 2, x, "+----------------------------------------------------------------------------------------------------+");
    attroff(COLOR_PAIR(1) | A_BOLD);
}

int main() {
    setlocale(LC_ALL, ""); // Activeaza suportul pentru Emoji/UTF-8
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

    // Activam atat click-ul cat si scroll up (BUTTON4) si scroll down (BUTTON5)
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

    bool ruleaza = true;
    MEVENT event; 
    StareAplicatie stareCurenta = MENIU_PRINCIPAL;
    int indexSelectat = -1; 
    
    int offsetFilme = 0; 
    int maxFilmePeEcran = 3; 

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

        clear();
        
        if (stareCurenta == MENIU_PRINCIPAL) {
            afiseazaAntet(xLogo);
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
            
            if (offsetFilme > 0) {
                deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16);
            }
            deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
            if (offsetFilme + maxFilmePeEcran < programAzi.size()) {
                deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16);
            }
            
            attron(COLOR_PAIR(1));
            mvprintw(24, xLogo - 6, "-> Foloseste rotita de mouse sau click pt a naviga!");
            attroff(COLOR_PAIR(1));
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
            attron(COLOR_PAIR(3));
            mvprintw(7, xLogo + 16, "[ SECTIUNEA BAR ]");
            attroff(COLOR_PAIR(3));
            mvprintw(9, xLogo + 10, "Suc: 10 RON  |  Nachos: 15 RON");
            deseneazaButonD(19, xButon, "0. Inapoi la Meniu", 4, 28);
        }
        else if (stareCurenta == PROMOTII) {
            afiseazaAntet(xLogo);
            attron(COLOR_PAIR(3));
            mvprintw(7, xLogo + 17, "[ PROMOTII ]");
            attroff(COLOR_PAIR(3));
            mvprintw(9, xLogo + 10, "1+1 Gratis la orice bilet martea!");
            deseneazaButonD(19, xButon, "0. Inapoi la Meniu", 4, 28);
        }
        
        refresh(); 

        int actiune = getch();
        if (actiune == KEY_RESIZE) continue; 
        
        if (stareCurenta == BILETE) {
            if (actiune == KEY_UP && offsetFilme > 0) {
                offsetFilme--;
                continue;
            }
            if (actiune == KEY_DOWN && offsetFilme + maxFilmePeEcran < programAzi.size()) {
                offsetFilme++;
                continue;
            }
        }

        if (actiune == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                
                // --- MAGIA PENTRU ROTITA DE MOUSE ---
                if (stareCurenta == BILETE) {
                    if ((event.bstate & BUTTON4_PRESSED) && offsetFilme > 0) {
                        offsetFilme--; // Scroll UP
                        continue;
                    }
                    if ((event.bstate & BUTTON5_PRESSED) && offsetFilme + maxFilmePeEcran < programAzi.size()) {
                        offsetFilme++; // Scroll DOWN
                        continue;
                    }
                }

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
                            if (offsetFilme > 0 && event.x >= xControls && event.x <= xControls + 16) {
                                offsetFilme--;
                            }
                            else if (event.x >= xControls + 20 && event.x <= xControls + 40) {
                                stareCurenta = MENIU_PRINCIPAL;
                                offsetFilme = 0; 
                            }
                            else if (offsetFilme + maxFilmePeEcran < programAzi.size() && event.x >= xControls + 44 && event.x <= xControls + 60) {
                                offsetFilme++;
                            }
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
                    else {
                        if (event.y >= 19 && event.y <= 21 && event.x >= xButon && event.x <= xButon + 27) {
                            stareCurenta = MENIU_PRINCIPAL; 
                        }
                    }
                }
            }
        }
        else if (actiune == '0') {
            if (stareCurenta == MENIU_PRINCIPAL) ruleaza = false;
            else if (stareCurenta == SELECTIE_LOCURI) stareCurenta = BILETE;
            else stareCurenta = MENIU_PRINCIPAL;
        }
    }

    endwin();              
    printf("\033[?1000l\n"); 
    return 0;
}