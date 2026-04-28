#pragma once 
#include <string>

using namespace std;

class Film {
private:
    string titlu;
    int durataMinute;
    double pretBilet;

public:
    Film(string t, int d, double p) {
        titlu = t;
        durataMinute = d;
        pretBilet = p;
    }

    string getTitlu() { return titlu; }
    int getDurata() { return durataMinute; }
    double getPret() { return pretBilet; }
};