#pragma once 
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Sala {
private:
    string idSala; 
    string numeSala;
    int randuri;
    int locuriPeRand;
    vector<string> matriceLayout;
    bool esteVIP;

public:
    Sala() : idSala(""), numeSala(""), randuri(0), locuriPeRand(0), esteVIP(false) {} 

    Sala(string id, string nume, string caleFisier, bool vip = false) {
        idSala = id;
        numeSala = nume;
        randuri = 0;
        locuriPeRand = 0;
        esteVIP = vip;

        ifstream fisier(caleFisier);
        if (fisier.is_open()) {
            string linie;
            while (getline(fisier, linie)) {
                if (!linie.empty()) {
                    matriceLayout.push_back(linie);
                    if ((int)linie.length() > locuriPeRand) {
                        locuriPeRand = linie.length();
                    }
                }
            }
            randuri = matriceLayout.size();
            fisier.close();
        } else {
            randuri = 8;
            locuriPeRand = 13;
            matriceLayout = vector<string>(randuri, "OOOOOO.OOOOOO");
        }
    }

    string getId() const { return idSala; }
    string getNume() const { return numeSala; }
    int getRanduri() const { return randuri; }
    int getLocuriPeRand() const { return locuriPeRand; }
    bool getEsteVIP() const { return esteVIP; }
    
    char getTipCelulă(int r, int c) const {
        if (r >= 0 && r < randuri && c >= 0 && c < locuriPeRand) {
            return matriceLayout[r][c];
        }
        return '.';
    }
};