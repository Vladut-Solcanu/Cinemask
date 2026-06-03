Pentru a asigura funcționarea corectă a interfeței grafice și a motorului de calcul dinamic al prețurilor, aplicația trebuie rulată într-un mediu Linux (ex. Ubuntu, Debian) sau prin WSL (Windows Subsystem for Linux).

Pasul 1: Pregătirea mediului de lucru
Descărcați folderul proiectului (sau clonați repository-ul) pe mașina locală.

Deschideți o fereastră de terminal (Consolă).

Navigați către directorul principal al proiectului folosind comanda: cd Cinemask

Pasul 2: Instalarea dependențelor
Aplicația utilizează biblioteca grafică ncurses. Pe lângă aceasta, pentru a testa eficient motorul de promoții (care depinde de ziua și ora sistemului), vom folosi utilitarul faketime. Această bibliotecă de Linux permite simularea unei anumite date calendaristice pentru un singur proces, fără a modifica ceasul global al calculatorului.

Rulați următoarea comandă pentru a instala ambele pachete:

sudo apt-get update
sudo apt-get install libncurses5-dev libncursesw5-dev faketime

Pasul 3: Rularea Standard (Default)
Aceasta este metoda clasică de rulare. Aplicația va fi compilată și executată folosind data și ora reală a sistemului de operare.

g++ main.cpp -o cinema -lncurses
./cinema

Pasul 4: Testarea Motorului de Promoții (Simulare Temporală)
Pentru a audita sistemul de oferte (care activează reduceri doar în anumite zile sau intervale orare), putem „călători în timp” folosind biblioteca faketime.

Mai jos sunt comenzile de testare care compilează codul și îl rulează direct la o dată specifică, pentru a observa cum interfața și coșul de cumpărături modifică automat prețurile:

1. Testare promoție de Vineri (ex. 29 Mai 2026, ora 10:00)
Declanșează reducerile standard aplicate în zilele de vineri.

g++ main.cpp -o cinema -lncurses && faketime '2026-05-29 10:00:00' ./cinema

2. Testare eveniment special "Vineri 13" (ex. 13 Noiembrie 2026, ora 10:00)
Declanșează ofertele speciale sau easter-eggs programate exclusiv pentru zilele de Vineri 13.

g++ main.cpp -o cinema -lncurses && faketime '2026-11-13 10:00:00' ./cinema

3. Testare ofertă "Happy Hour" Sâmbăta (ex. 30 Mai 2026, ora 15:00)
Declanșează reducerile de la Snack Bar (sau prețurile reduse la bilete) valabile strict în weekend, pe parcursul după-amiezii.

g++ main.cpp -o cinema -lncurses && faketime '2026-05-30 15:00:00' ./cinema

