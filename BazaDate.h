#ifndef BAZADATE_H
#define BAZADATE_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "Film.h"
#include "Sala.h"
#include "Proiectie.h"
#include "Gestiune.h"

using namespace std;

namespace BazaDate {

    vector<Sala> incarcaSali() {
        vector<Sala> sali;
        sali.push_back(Sala("IMAX", "Sala IMAX", "sala_imax.txt"));
        sali.push_back(Sala("VIP",  "Sala VIP", "sala_vip.txt", true));
        sali.push_back(Sala("STD1", "Sala Standard 1", "sala_std1.txt"));
        sali.push_back(Sala("STD2", "Sala Standard 2", "sala_std2.txt"));
        return sali;
    }

    vector<Film> incarcaFilme() {
        vector<Film> filme;
        ifstream f("filme.txt");
        if (f.is_open()) {
            string linie;
            while (getline(f, linie)) {
                if (linie.empty()) continue;
                stringstream ss(linie);
                string t, dStr, pStr, premStr, g;
                getline(ss, t, '|'); getline(ss, dStr, '|'); getline(ss, pStr, '|'); 
                getline(ss, premStr, '|'); getline(ss, g, '|'); 
                g.erase(remove(g.begin(), g.end(), '\r'), g.end()); 
                filme.push_back(Film(t, atoi(dStr.c_str()), atof(pStr.c_str()), (premStr == "1"), g));
            }
            f.close();
        }
        return filme;
    }

    vector<Proiectie> incarcaProgram(int ziuaCurenta, const vector<Film>& filme, const vector<Sala>& sali) {
        vector<Proiectie> prog;
        ifstream f("program.txt");
        if (f.is_open()) {
            string linie;
            while (getline(f, linie)) {
                if (linie.empty()) continue;
                stringstream ss(linie);
                string ziStr, titluProg, oraProg, idSalaProg;
                getline(ss, ziStr, '|'); getline(ss, titluProg, '|'); 
                getline(ss, oraProg, '|'); getline(ss, idSalaProg, '|');
                idSalaProg.erase(remove(idSalaProg.begin(), idSalaProg.end(), '\r'), idSalaProg.end());

                if (atoi(ziStr.c_str()) == ziuaCurenta) {
                    Film filmGasit = filme[0]; 
                    for (auto& f_obj : filme) if (f_obj.getTitlu() == titluProg) { filmGasit = f_obj; break; }
                    Sala salaGasita = sali[0]; 
                    for (auto& s_obj : sali) if (s_obj.getId() == idSalaProg) { salaGasita = s_obj; break; }
                    prog.push_back(Proiectie(filmGasit, salaGasita, oraProg));
                }
            }
            f.close();
        }
        return prog;
    }

    void incarcaRezervari(vector<Proiectie>& prog) {
        ifstream f("rezervari_locuri.txt");
        if (f.is_open()) {
            string linie;
            while (getline(f, linie)) {
                if (linie.empty()) continue;
                stringstream ss(linie);
                string tFilm, oraFilM, numeS, randStr, locStr;
                getline(ss, tFilm, '|'); getline(ss, oraFilM, '|'); 
                getline(ss, numeS, '|'); getline(ss, randStr, '|'); getline(ss, locStr, '|');
                int r = atoi(randStr.c_str()); int c = atoi(locStr.c_str());
                for (auto& pr : prog) {
                    if (pr.getFilm().getTitlu() == tFilm && pr.getOra() == oraFilM && pr.getSala().getId() == numeS) {
                        pr.rezervaLoc(r, c); break;
                    }
                }
            }
            f.close();
        }
    }

    vector<ProdusBar> incarcaBar() {
        vector<ProdusBar> bar;
        ifstream f("bar.txt");
        if (f.is_open()) {
            string linie;
            while (getline(f, linie)) {
                if (linie.empty()) continue;
                stringstream ss(linie);
                string nume, pretStr;
                getline(ss, nume, '|'); getline(ss, pretStr, '|');
                bar.push_back(ProdusBar(nume, atof(pretStr.c_str())));
            }
            f.close();
        }
        return bar;
    }

    vector<Promotie> incarcaPromotii() {
        vector<Promotie> prom;
        ifstream f("promotii.txt");
        if (f.is_open()) {
            string linie;
            while (getline(f, linie)) {
                if (linie.empty()) continue;
                stringstream ss(linie);
                string titlu, desc, zi, os, oe, te, tinta, val;
                getline(ss, titlu, '|'); getline(ss, desc, '|'); getline(ss, zi, '|'); 
                getline(ss, os, '|'); getline(ss, oe, '|'); getline(ss, te, '|'); 
                getline(ss, tinta, '|'); getline(ss, val, '|');
                val.erase(remove(val.begin(), val.end(), '\r'), val.end());
                val.erase(remove(val.begin(), val.end(), '\n'), val.end());
                prom.push_back(Promotie(titlu, desc, zi, os, oe, te, tinta, atof(val.c_str())));
            }
            f.close();
        }
        return prom;
    }

    string verificaParola() {
        string parolaSalvata = "cinema2026"; 
        ifstream fPassIn("parola.txt");
        if (fPassIn.is_open()) { getline(fPassIn, parolaSalvata); fPassIn.close(); } 
        else { ofstream fPassOut("parola.txt"); fPassOut << parolaSalvata; fPassOut.close(); }
        return parolaSalvata;
    }

    vector<string> genereazaIstoricBilete() {
        vector<string> istoric;
        ifstream fb("vanzari_bilete.txt"); string linie;
        
        // Structura auxiliara pentru a tine minte datele originale frumos formatate
        struct DateBilet { string t, ora, sala, data; };
        map<string, int> contorBilete; 
        map<string, float> incasariBilete;
        map<string, DateBilet> detaliiBilete;

        while(getline(fb, linie)) {
            if(!linie.empty()) {
                stringstream ss(linie); string t, ora, sala, gen, dataCumpararii, pretStr;
                if(getline(ss, t, '|') && getline(ss, ora, '|') && getline(ss, sala, '|') && getline(ss, gen, '|') && getline(ss, dataCumpararii, '|') && getline(ss, pretStr, '|')) {
                    
                    // Secretul sortarii cronologice: formatam cheia ca YYYYMMDD_HH:MM
                    string an = dataCumpararii.substr(6, 4);
                    string luna = dataCumpararii.substr(3, 2);
                    string zi = dataCumpararii.substr(0, 2);
                    string cheieSortare = an + luna + zi + "|" + ora + "|" + t + "|" + sala;
                    
                    contorBilete[cheieSortare]++; 
                    incasariBilete[cheieSortare] += atof(pretStr.c_str()); 
                    detaliiBilete[cheieSortare] = {t, ora, sala, dataCumpararii};
                }
            }
        }
        fb.close();
        
        // Map-ul ne va da elementele direct in ordinea cronologica perfecta
        for (auto const& pereche : contorBilete) {
            DateBilet d = detaliiBilete[pereche.first];
            char buf[200];
            snprintf(buf, sizeof(buf), "%-25.25s | %-5s | %-12.12s | %-10s | %-14s | %.2f RON", 
                     d.t.c_str(), d.ora.c_str(), d.sala.c_str(), d.data.c_str(), 
                     ("Bilete: " + to_string(pereche.second)).c_str(), incasariBilete[pereche.first]);
            istoric.push_back(string(buf));
        }
        return istoric;
    }

    vector<string> genereazaIstoricBar() {
        vector<string> istoric;
        ifstream fbar("vanzari_bar.txt"); string linie;
        
        // Structura auxiliara pentru a tine minte datele originale pentru afisare
        struct DateBar { string nume, data, ora; };
        map<string, int> contorBar; 
        map<string, float> incasariBar;
        map<string, DateBar> detaliiBar;

        while(getline(fbar, linie)) {
            if(!linie.empty()) {
                stringstream ss(linie);
                string nume, data, ora, pretStr;
                
                // Format fisier: Nume|Data|Ora|Pret
                if(getline(ss, nume, '|') && getline(ss, data, '|') && getline(ss, ora, '|') && getline(ss, pretStr, '|')) {
                    
                    // Secretul sortarii cronologice: YYYYMMDD|HH:MM|Nume
                    string an = data.substr(6, 4);
                    string luna = data.substr(3, 2);
                    string zi = data.substr(0, 2);
                    string cheieSortare = an + luna + zi + "|" + ora + "|" + nume;
                    
                    contorBar[cheieSortare]++;
                    incasariBar[cheieSortare] += atof(pretStr.c_str());
                    detaliiBar[cheieSortare] = {nume, data, ora};
                }
            }
        }
        fbar.close();

        // Acum iteram prin map-ul care este sortat automat perfect cronologic
        for (auto const& pereche : contorBar) {
            DateBar d = detaliiBar[pereche.first];
            char buf[200];
            snprintf(buf, sizeof(buf), "%-30.30s | %-10s | %-5s | Buc: %-3d | %.2f RON", 
                     d.nume.c_str(), d.data.c_str(), d.ora.c_str(), pereche.second, incasariBar[pereche.first]);
            istoric.push_back(string(buf));
        }
        return istoric;
    }

    void finalizeazaComanda(const vector<ProdusCos>& cos, vector<Proiectie>& programAzi) {
        ofstream fileBilete("vanzari_bilete.txt", ios::app); 
        ofstream fileBar("vanzari_bar.txt", ios::app); 
        ofstream fileLocuri("rezervari_locuri.txt", ios::app); 
        
        for (auto& produs : cos) {
            if (produs.tip == 0) {
                if(fileBilete.is_open()) fileBilete << produs.logAdmin << " | " << produs.pret << " RON\n";
                if(fileLocuri.is_open() && produs.indexProiectie != -1) {
                    auto& pr = programAzi[produs.indexProiectie];
                    fileLocuri << pr.getFilm().getTitlu() << "|" << pr.getOra() << "|" << pr.getSala().getId() << "|" << produs.randScaun << "|" << produs.locScaun << "\n";
                }
            } else if (produs.tip == 1 && fileBar.is_open()) { 
                fileBar << produs.logAdmin << "|" << produs.pret << "\n"; 
            }
        }
        if(fileBilete.is_open()) fileBilete.close(); 
        if(fileBar.is_open()) fileBar.close(); 
        if(fileLocuri.is_open()) fileLocuri.close();
    }
}
#endif