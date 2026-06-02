#ifndef ECRANE_H
#define ECRANE_H

#include <ncurses.h>
#include <string>
#include <vector>
#include "Interfata.h"
#include "Gestiune.h"
#include "Proiectie.h"

using namespace std;

namespace Ecrane {

    void afiseazaMeniuPrincipal(int xLogo, int xButon) {
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

    void afiseazaAdminLogin(int xLogo, int xButon, string parolaIntrodusa, bool eroareParola) {
        afiseazaAntet(xLogo);
        attron(COLOR_PAIR(4) | A_BOLD); mvprintw(7, xLogo + 9, "[ AUTENTIFICARE ADMINISTRATOR ]"); attroff(COLOR_PAIR(4) | A_BOLD);
        mvprintw(10, (COLS - 22) / 2, "Introduceti parola:");
        int boxX = (COLS - 20) / 2; mvprintw(12, boxX - 2, "[                    ]");
        string masca(parolaIntrodusa.length(), '*');
        attron(COLOR_PAIR(3) | A_BOLD); mvprintw(12, boxX, "%s", masca.c_str()); attroff(COLOR_PAIR(3) | A_BOLD);
        if (eroareParola) { attron(COLOR_PAIR(4) | A_BOLD); mvprintw(14, (COLS - 17) / 2, "Parola incorecta!"); attroff(COLOR_PAIR(4) | A_BOLD); }
        deseneazaButonD(16, xButon - 5, "AUTENTIFICARE", 3, 18);
        deseneazaButonD(16, xButon + 15, "INAPOI", 4, 18);
    }

    void afiseazaAdminMeniu(int xLogo, int xButon) {
        afiseazaAntet(xLogo);
        attron(COLOR_PAIR(4) | A_BOLD); mvprintw(7, xLogo + 13, "[ PANOU DE GESTIUNE ]"); attroff(COLOR_PAIR(4) | A_BOLD);
        deseneazaButonD(11, xButon, "1. Vanzari Bilete", 3, 28);
        deseneazaButonD(15, xButon, "2. Vanzari Snack Bar", 3, 28);
        deseneazaButonD(19, xButon, "0. Inapoi la Public", 4, 28);
    }

    void afiseazaBilete(int xLogo, int xButonFilm, int latimeButonFilm, vector<Proiectie>& programAzi, int offsetFilme, int maxFilmePeEcran, const vector<Promotie>& listaPromotii, tm* acum_now, int timpCurentMin) {
        afiseazaAntet(xLogo);
        if (xButonFilm >= 34 && (xButonFilm + latimeButonFilm + 40) < COLS) {
            deseneazaMascaHorror(7, xButonFilm - 34, 4); deseneazaMascaNoua(6, xButonFilm + latimeButonFilm + 4, 1);
        }
        attron(COLOR_PAIR(3) | A_BOLD); mvprintw(7, xLogo - 2, "[ SECTIUNEA BILETE - PROGRAMUL MULTIPLEXULUI ]"); attroff(COLOR_PAIR(3) | A_BOLD);
        
        for(int i = 0; i < maxFilmePeEcran; i++) {
            int indexReal = offsetFilme + i; if(indexReal >= programAzi.size()) break; 
            string tagPremiera = programAzi[indexReal].getFilm().getEstePremiera() ? " [PREMIERA]" : "";
            string tagVIP = programAzi[indexReal].getSala().getEsteVIP() ? " [VIP]" : "";
            float pretCalculat = obtinePretBiletDinamic(programAzi[indexReal], 0, listaPromotii, acum_now->tm_wday, acum_now->tm_hour, acum_now->tm_min, acum_now->tm_mday);
            int hFilm = atoi(programAzi[indexReal].getOra().substr(0, 2).c_str()); int mFilm = atoi(programAzi[indexReal].getOra().substr(3, 2).c_str());
            int timpFilmMin = hFilm * 60 + mFilm; int dif = timpCurentMin - timpFilmMin;
            
            int culoareButon = 3; 
            if (dif >= 0 && dif <= 30) culoareButon = 4; 
            else if (programAzi[indexReal].getSala().getEsteVIP()) culoareButon = 2; 
            else if (programAzi[indexReal].getFilm().getEstePremiera()) culoareButon = 6; 

            char buffer[120];
            snprintf(buffer, sizeof(buffer), "[%s] %s%s%s | %s - %.2f RON", programAzi[indexReal].getOra().c_str(), programAzi[indexReal].getFilm().getTitlu().c_str(), tagVIP.c_str(), tagPremiera.c_str(), programAzi[indexReal].getSala().getId().c_str(), pretCalculat);
            deseneazaButonD(9 + (i * 4), xButonFilm, string(buffer), culoareButon, latimeButonFilm);
        }
        int yControls = 21; int xControls = (COLS - 60) / 2; 
        if (offsetFilme > 0) deseneazaButonD(yControls, xControls, "/\\ SUS /\\", 2, 16);
        deseneazaButonD(yControls, xControls + 20, "0. Inapoi", 4, 20);
        if (offsetFilme + maxFilmePeEcran < programAzi.size()) deseneazaButonD(yControls, xControls + 44, "\\/ JOS \\/", 2, 16);
    }

    void afiseazaSala(int indexSelectat, Proiectie& proiectie, const vector<pair<int, int>>& locuriSelectateCurent, const vector<Promotie>& listaPromotii, tm* acum_now, int xButon) {
        int xSalaCentrat = (COLS - (proiectie.getSala().getLocuriPeRand() * 6)) / 2; if (xSalaCentrat < 0) xSalaCentrat = 0;
        deseneazaEcranCinema(2, (COLS - 50) / 2);
        attron(COLOR_PAIR(2) | A_BOLD);
        string detalii = "Film: " + proiectie.getFilm().getTitlu() + " (" + proiectie.getFilm().getGen() + ") | " + proiectie.getSala().getNume();
        mvprintw(6, (COLS - detalii.length()) / 2, "%s", detalii.c_str()); attroff(COLOR_PAIR(2) | A_BOLD);

        int startY = 8; 
        for (int i = 0; i < proiectie.getSala().getRanduri(); i++) {
            float pretRand = obtinePretBiletDinamic(proiectie, i, listaPromotii, acum_now->tm_wday, acum_now->tm_hour, acum_now->tm_min, acum_now->tm_mday);
            int culoareLoc = 3; 
            if (proiectie.getSala().getEsteVIP()) culoareLoc = 2;
            else if (proiectie.getFilm().getEstePremiera() && acum_now->tm_wday != 5) { 
                if (i < 5) culoareLoc = 3; else if (i < 8) culoareLoc = 1; else culoareLoc = 6;             
            }
            attron(COLOR_PAIR(culoareLoc)); mvprintw(startY + (i * 2), xSalaCentrat - 22, "R%2d (%.0f RON)", i + 1, pretRand); attroff(COLOR_PAIR(culoareLoc));

            int scaunRealContor = 1; 
            for (int j = 0; j < proiectie.getSala().getLocuriPeRand(); j++) {
                int posX = xSalaCentrat + (j * 6);
                if (proiectie.getSala().getTipCelulă(i, j) == '.') continue;
                if (!proiectie.esteLiber(i, j)) { attron(COLOR_PAIR(4) | A_BOLD); mvprintw(startY + (i * 2), posX, "[ XX ]"); attroff(COLOR_PAIR(4) | A_BOLD); scaunRealContor++; } 
                else if (esteLocSelectat(i, j, locuriSelectateCurent)) { attron(COLOR_PAIR(5) | A_DIM); mvprintw(startY + (i * 2), posX, "[ SS ]"); attroff(COLOR_PAIR(5) | A_DIM); scaunRealContor++; }
                else { attron(COLOR_PAIR(culoareLoc)); mvprintw(startY + (i * 2), posX, "[ %02d ]", scaunRealContor); attroff(COLOR_PAIR(culoareLoc)); scaunRealContor++; }
            }
        }
        int yButoane = startY + (proiectie.getSala().getRanduri() * 2) + 2;
        deseneazaButonD(yButoane + 1, xButon - 16, "0. Inapoi la Filme", 4, 28);
        if (!locuriSelectateCurent.empty()) deseneazaButonD(yButoane + 1, xButon + 16, "-> ADAUGA IN COS <-", 3, 28);
    }
}
#endif