#ifndef GESTIUNE_H
#define GESTIUNE_H

#include <string>
#include <vector>
#include <cstdlib>
#include "Proiectie.h"

using namespace std;

struct ProdusBar {
    string nume;
    float pret;
    ProdusBar(string n, float p) : nume(n), pret(p) {}
};

struct Promotie {
    string titlu;
    string descriere;
    string zi;        
    string oraStart;  
    string oraEnd;    
    string tipEfect;  
    string tinta;     
    float valoare;

    Promotie(string t, string d, string z, string os, string oe, string te, string tin, float v)
        : titlu(t), descriere(d), zi(z), oraStart(os), oraEnd(oe), tipEfect(te), tinta(tin), valoare(v) {}
};

struct ProdusCos {
    int tip; 
    string nume; 
    string logAdmin; 
    float pret;
    int indexProiectie;
    int randScaun;
    int locScaun;

    ProdusCos(int t, string n, string l, float p, int idx = -1, int r = -1, int c = -1) 
        : tip(t), nume(n), logAdmin(l), pret(p), indexProiectie(idx), randScaun(r), locScaun(c) {}
};

// Functii de calcul si validare reguli de business
bool esteLocSelectat(int i, int j, const vector<pair<int, int>>& lista) {
    for (auto loc : lista) {
        if (loc.first == i && loc.second == j) return true;
    }
    return false;
}

bool estePromotieActiva(const Promotie& p, int ziCurenta, int oraCurenta, int minCurent, int ziLuna) {
    if (p.zi != "ALL") {
        if (p.zi == "V13") {
            if (ziCurenta != 5 || ziLuna != 13) return false;
        } else {
            if (ziCurenta != atoi(p.zi.c_str())) return false;
        }
    }
    if (p.oraStart != "ALL" && p.oraEnd != "ALL") {
        int hStart = atoi(p.oraStart.substr(0, 2).c_str());
        int mStart = atoi(p.oraStart.substr(3, 2).c_str());
        int hEnd = atoi(p.oraEnd.substr(0, 2).c_str());
        int mEnd = atoi(p.oraEnd.substr(3, 2).c_str());

        int timpCurentMin = oraCurenta * 60 + minCurent;
        int timpStartMin = hStart * 60 + mStart;
        int timpEndMin = hEnd * 60 + mEnd;

        if (timpCurentMin < timpStartMin || timpCurentMin >= timpEndMin) return false;
    }
    return true;
}

float obtinePretBiletDinamic(Proiectie& pr, int rand, const vector<Promotie>& promotii, int ziC, int oraC, int minC, int ziL) {
    float pretBaza = pr.getPretLoc(rand);
    for (const auto& p : promotii) {
        if (estePromotieActiva(p, ziC, oraC, minC, ziL)) {
            if (p.tipEfect == "BILETE_FIX") {
                if (p.tinta == "ALL") return p.valoare;
                if (p.tinta == "Horror" && pr.getFilm().getGen() == "Horror") return p.valoare;
            }
        }
    }
    return pretBaza;
}

float obtinePretBarDinamic(string numeProdus, float pretBaza, const vector<Promotie>& promotii, int ziC, int oraC, int minC, string& tagPromotie) {
    tagPromotie = "";
    for (const auto& p : promotii) {
        if (estePromotieActiva(p, ziC, oraC, minC, -1)) {
            if (p.tipEfect == "BAR_REDUCERE") {
                if (p.tinta == "ALL" || (numeProdus.find(p.tinta) != string::npos && !(numeProdus.find("Combo") != string::npos))) {
                    tagPromotie = " [" + p.titlu + "]";
                    return pretBaza * (1.0f - (p.valoare / 100.0f));
                }
            }
        }
    }
    return pretBaza;
}

#endif