#ifndef INTERFATA_H
#define INTERFATA_H

#include <ncurses.h>
#include <string>
#include <vector>

using namespace std;

// Functiile de desenat elemente grafice
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

#endif