#pragma once
#include <string>
#include <vector>
#include <ctime> // Necesar pentru timp
#include "Film.h"
#include "Sala.h"

using namespace std;

class Proiectie {
private:
    Film filmulRulat;
    Sala salaAsignata;
    string oraStart;
    vector<vector<bool>> scauneOcupate; 

public:
    Proiectie(Film f, Sala s, string ora) : filmulRulat(f), salaAsignata(s) {
        oraStart = ora;
        scauneOcupate = vector<vector<bool>>(salaAsignata.getRanduri(), vector<bool>(salaAsignata.getLocuriPeRand(), false));
    }

    Film getFilm() { return filmulRulat; }
    Sala getSala() { return salaAsignata; }
    string getOra() { return oraStart; }

    float getPretLoc(int rand) {
        time_t t = time(0);
        tm* acum = localtime(&t);
        
        int ziuaSaptamanii = acum->tm_wday; 
        int ziuaLunii = acum->tm_mday;       

        // 1. Promotie Vineri 13 Horror
        if (ziuaSaptamanii == 5 && ziuaLunii == 13 && filmulRulat.getGen() == "Horror") {
            return 0.00; 
        }

        // 2. Pret fix VIP
        if (salaAsignata.getEsteVIP()) {
            return 100.00;
        }

        // 3. Promotie Generala Vineri in restul salilor
        if (ziuaSaptamanii == 5 && (salaAsignata.getId() == "IMAX" || salaAsignata.getId().find("STD") != string::npos)) {
            return 10.00;
        }

        // 4. Calcul standard in functie de zone si premiera
        float pretFinal = filmulRulat.getPretBaza();
        if (filmulRulat.getEstePremiera()) {
            if (rand < 5) pretFinal += 0.00; 
            else if (rand < 8) pretFinal += 5.00;
            else pretFinal += 10.00;
        }
        return pretFinal;
    }

    bool esteLiber(int rand, int loc) {
        if (salaAsignata.getTipCelulă(rand, loc) == 'O') {
            return scauneOcupate[rand][loc] == false;
        }
        return false; 
    }

    void rezervaLoc(int rand, int loc) {
        if (esteLiber(rand, loc)) {
            scauneOcupate[rand][loc] = true;
        }
    }

    // NOU: Pentru cand golim cosul sau stergem un bilet
    void elibereazaLoc(int rand, int loc) {
        if (rand >= 0 && rand < salaAsignata.getRanduri() && loc >= 0 && loc < salaAsignata.getLocuriPeRand()) {
            scauneOcupate[rand][loc] = false;
        }
    }
};