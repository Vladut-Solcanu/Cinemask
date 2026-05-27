#pragma once
#include <string>
#include <vector>
#include <ctime> // NOU: Pentru timp si data
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
        // Preluam data curenta a sistemului
        time_t t = time(0);
        tm* acum = localtime(&t);
        
        int ziuaSaptamanii = acum->tm_wday; // 5 = Vineri
        int ziuaLunii = acum->tm_mday;       // Ziua din calendar

        // REGULA EXTRA: Prima Vineri 13 din an -> Toate filmele Horror sunt GRATIS (0 RON)
        // (Nota: Pentru a fi "prima" din an, verificam doar luna Ianuarie/tm_mon == 0, sau lasam pe orice vineri 13)
        if (ziuaSaptamanii == 5 && ziuaLunii == 13 && filmulRulat.getGen() == "Horror") {
            return 0.00; 
        }

        // Regula Suprema anterioara: Daca e in Sala VIP, costa fix 100 RON orice scaun (VIP-ul nu are reduceri standard)
        if (salaAsignata.getEsteVIP()) {
            return 100.00;
        }

        // REGULA NOUĂ: Daca este Vineri, in salile Standard si IMAX biletul devine fix 10 RON!
        if (ziuaSaptamanii == 5 && (salaAsignata.getId() == "IMAX" || salaAsignata.getId() == "STD1" || salaAsignata.getId() == "STD2")) {
            return 10.00;
        }

        // Daca nu e vineri, se aplica regulile tale clasice de pret de baza si premiere
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

    bool rezervaLoc(int rand, int loc) {
        if (esteLiber(rand, loc)) {
            scauneOcupate[rand][loc] = true;
            return true;
        }
        return false;
    }
};