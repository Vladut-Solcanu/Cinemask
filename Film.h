#pragma once
#include <string>

using namespace std;

class Film {
private:
    string titlu;
    int durata;
    float pretBaza;
    bool estePremiera;
    string gen; // NOU: Genul filmului

public:
    Film() : titlu(""), durata(0), pretBaza(0.0), estePremiera(false), gen("") {}

    Film(string t, int d, float p, bool premiera, string g) {
        titlu = t;
        durata = d;
        pretBaza = p;
        estePremiera = premiera;
        gen = g;
    }

    string getTitlu() const { return titlu; }
    int getDurata() const { return durata; }
    float getPretBaza() const { return pretBaza; }
    bool getEstePremiera() const { return estePremiera; }
    string getGen() const { return gen; } // NOU: Getter pentru gen
};