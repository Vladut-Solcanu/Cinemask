#pragma once
#include <string>
#include <vector>
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
        // Generam scaunele libere pentru ACEASTA ora specifica
        scauneOcupate = vector<vector<bool>>(salaAsignata.getRanduri(), vector<bool>(salaAsignata.getLocuriPeRand(), false));
    }

    Film getFilm() { return filmulRulat; }
    Sala getSala() { return salaAsignata; }
    string getOra() { return oraStart; }

    bool esteLiber(int rand, int loc) {
        return scauneOcupate[rand][loc] == false;
    }

    bool rezervaLoc(int rand, int loc) {
        if (esteLiber(rand, loc)) {
            scauneOcupate[rand][loc] = true;
            return true;
        }
        return false;
    }
};