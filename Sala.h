#pragma once 
#include <string>

using namespace std;

class Sala {
private:
    string numeSala;
    int randuri;
    int locuriPeRand;

public:
    Sala(string nume, int r, int l) {
        numeSala = nume;
        randuri = r;
        locuriPeRand = l;
    }

    string getNume() { return numeSala; }
    int getRanduri() { return randuri; }
    int getLocuriPeRand() { return locuriPeRand; }
};