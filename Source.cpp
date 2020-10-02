#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

using namespace std;

typedef struct Produkty {
	int id;
	char nazov[20];
	char vyrobca[20];
	int sklad;
	int cena;
} PRODUKT;

typedef struct Zakaznik {
	string meno;
	string priezvisko;
	int rozpocet;
	PRODUKT kosik[50];
} ZAKAZNIK;

PRODUKT* nacitaj_subor(string nazov_suboru) {		//procedura nacitania udajov zo suboru
	fstream zoznam(nazov_suboru, ios::in);
	short int pocet;
	int i;

	if (!zoznam.is_open() ) {		//test existencie suboru
		cout << "Subor sa neda otvorit. \n";
		exit(1);
	}

	zoznam >> pocet;

	PRODUKT* produkty = new PRODUKT[pocet+1];	//dynamicka alokacia pola produktov   ....vytvoreny jeden sample produkt, kde si ulozim pocet produktov		
	for (i = 1; i <= pocet; i++) {
		zoznam >> produkty[i].id;
		zoznam >> produkty[i].nazov;
		zoznam >> produkty[i].vyrobca;
		zoznam >> produkty[i].sklad;
		zoznam >> produkty[i].cena;
	}

	zoznam.close();
	produkty[0].id = pocet;		//pocitadlo mnozstva produktov
	return(produkty);
}

void predaj(ZAKAZNIK* osoba, PRODUKT* produkty);

void spoznaj_zakaznika(ZAKAZNIK* osoba, PRODUKT* produkty) {
	int pyt;		//premenna na kontrolu zberu informacii
	int i;
	osoba->rozpocet = 0;

	cout << "Vitajte v obchode.\nTento program o vas zbiera informacie, prajete si pokracovat?\n 1 = ano \t 0 = nie \n";
	cin >> pyt;
	if (pyt == 0) {
		cout << "KONIEC\n";
		exit(1);
	}
	else if (pyt == 1) {
	}
	else {
		cout << "Nevyjadrenie suhlasu povazujeme za nesuhlas.\n";
		exit(1);
	}


	for (i = 1; i <= produkty[0].id; i++) {
		osoba->kosik[i].id = i;
		strcpy_s(osoba->kosik[i].nazov , 20 , produkty[i].nazov);
		strcpy_s(osoba->kosik[i].vyrobca , 20 , produkty[i].vyrobca);
		osoba->kosik[i].sklad = 0;							//vyprazdnenie kosika
		osoba->kosik[i].cena = produkty[i].cena;
	}

	cout << "Zadajte vase krstne meno: \n";
	cin >> osoba->meno;

	cout << "Zadajte vase priezvisko: \n";
	cin >> osoba->priezvisko;

	cout << "Zadajte vas rozpocet v celych cislach: " << endl;

	do  {
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cin >> osoba->rozpocet;
		if (osoba->rozpocet <= 0) {
			cout << "Neplatny vstup. Zadajte kladny rozpocet v celych cislach. \n";
		}
		
	} while (osoba->rozpocet <= 0);

}

void blocek(ZAKAZNIK* osoba, PRODUKT* produkty, int& platba) {
	int i;
	fstream blok("blocek.txt", ios::out);

	if (!blok.is_open()) {		//kontrola vytvorenia suboru
		cout << "Chyba ukladania blocka. \n";
		exit(1);
	}

	blok << "meno: " << osoba->meno << " \npriezvisko: " << osoba->priezvisko << " \n\n";
	blok << "Nakupene produkty: \nid" << string(10, ' ') << "nazov" << string(5, ' ') << "vyrobca" << string(5, ' ') << "zasoba" << string(5, ' ') << "cena\n";
	
	
	for (i = 1; i <= produkty[0].id; i++) {
		if (osoba->kosik[i].sklad != 0) {
			blok << setw(2) << osoba->kosik[i].id << setw(15) << osoba->kosik[i].nazov << setw(12) << osoba->kosik[i].vyrobca << setw(11) << osoba->kosik[i].sklad << setw(9) << osoba->kosik[i].cena << endl;
		}
	}

	blok << "\ncena nakupu: " << fixed << setprecision(2) << (float)platba << "\ncena bez DPH: " << fixed << setprecision(2) << (float)platba * 0.8 << endl;
	blok.close();

}

void vyber_tovaru(ZAKAZNIK* osoba, PRODUKT* produkty, int vysledky[]) {
	int id, i;
	int kusy = -1;

	while (true) {
		cout << "Zadajte id ziadaneho tovaru alebo 0 pre hlavnu ponuku: ";
		cin >> id;

		if (id == 0) {
			delete[] vysledky;
			predaj(osoba, produkty);
		}
		else {
			for (i = 1; i <= vysledky[0]; i++) {		//prehladava id najdenych produktov
				if (id == vysledky[i]) {

					if (produkty[id].sklad == 0) {				//kontrola, ci nie je vypredane
						cout << "Tovar je vypredany. Navrat do hlavnej ponuky. \n";
						delete[] vysledky;
						predaj(osoba, produkty);
					}

					do{		//opyta pocet kusov na kupenie a kontroluje zle hodnoty
						cout << "Zadajte ziadany pocet kusov. Pre navrat do hlavnej ponuky napiste 0: ";
						cin >> kusy;

						if (kusy == 0) {
							delete[] vysledky;
							predaj(osoba, produkty);
						}

						else if (produkty[id].sklad < kusy) {	//kontrola, ci je na sklade dost kusov
							kusy = -1;
							cout << "Nedostatok produktov na sklade, zostalo uz len " << produkty[id].sklad << " ks.\n";
						}

						else if ((osoba->kosik[id].cena * kusy) > osoba->rozpocet) {			//kontrola rozpoctu
							cout << "Nedostatocny rozpocet, zostava vam uz len " << osoba->rozpocet << ". Pre navrat do hlavnej ponuky napiste 0:\n";
							kusy = -1;
						}

					} while (kusy < 0);

					osoba->rozpocet -= osoba->kosik[id].cena * kusy;
					produkty[id].sklad -= kusy;
					osoba->kosik[0].sklad++;			//ukazujem, ze kosik nie je prazdny
					osoba->kosik[id].sklad += kusy;
					cout << "Uspesne kupenych " << kusy << "ks produktu " << (produkty[id]).nazov << " od vyrobcu " << (produkty[id]).vyrobca << ".\n";
					kusy = -1;
				}
			}
		}
	}



}

void predaj_nazov(ZAKAZNIK* osoba, PRODUKT* produkty) {


	string mproduktu;
	int i;
	int *vysledky = new int[produkty[0].id + 1];
	vysledky[0] = 0;			//deklaracia pocitadla spravnych vysledkov

	cout << "Zadajte nazov hladaneho produktu: \n";
	cin >> mproduktu;
	cout << "\nid" << string(10, ' ') << "nazov" << string(5, ' ') << "vyrobca" << string(5, ' ') << "zasoba" << string(5, ' ') << "cena\n";

	for (i = 1; i <= produkty[0].id; i++) {

		if (mproduktu.compare(produkty[i].nazov) == 0) {							//porovnanie hladaneho a vysledkov
			cout << setw(2) << produkty[i].id << setw(15) << produkty[i].nazov << setw(12) << produkty[i].vyrobca << setw(11) << produkty[i].sklad << setw(9) << produkty[i].cena << endl;
			vysledky[0]++;										//pocet spravnych vysledkov sa zvacsi o 1
			vysledky[vysledky[0]] = i;							//na dane miesto sa zapise id najdeneho produktu
			
		}
	}

	if (vysledky[0] == 0) {
		cout << "Hladany produkt nebol najdeny. Navrat do hlavnej ponuky. \n";
		predaj(osoba, produkty);
	}

	vyber_tovaru(osoba, produkty, vysledky);

	predaj(osoba, produkty);
}

void predaj_vyrobca(ZAKAZNIK* osoba, PRODUKT* produkty) {

	string mvyrobcu;
	int i;
	int* vysledky = new int[produkty[0].id + 1];
	vysledky[0] = 0;			//deklaracia pocitadla spravnych vysledkov

	cout << "Zadajte nazov hladaneho vyrobcu: \n";
	cin >> mvyrobcu;
	cout << "\nid" << string(10, ' ') << "nazov" << string(5, ' ') << "vyrobca" << string(5, ' ') << "zasoba" << string(5, ' ') << "cena\n";

	for (i = 1; i <= produkty[0].id; i++) {

		if (mvyrobcu.compare(produkty[i].vyrobca) == 0) {							//porovnanie hladaneho a vysledkov
			cout << setw(2) << produkty[i].id << setw(15) << produkty[i].nazov << setw(12) << produkty[i].vyrobca << setw(11) << produkty[i].sklad << setw(9) << produkty[i].cena << endl;
			vysledky[0]++;										//pocet spravnych vysledkov sa zvacsi o 1
			vysledky[vysledky[0]] = i;							//na dane miesto sa zapise id najdeneho produktu
			
		}
	}

	if (vysledky[0] == 0) {
		cout << "Hladany vyrobca nebol najdeny. Navrat do hlavnej ponuky. \n" ;
		predaj(osoba, produkty);
	}

	else {
		vyber_tovaru(osoba, produkty, vysledky);
	}

	predaj(osoba, produkty);
}

void predaj(ZAKAZNIK* osoba, PRODUKT* produkty) {
	int i;
	int volba = 0;
	
	cout <<"\n################################################################ \n";
	cout << "Vazeny/a " << osoba->meno << " " << osoba->priezvisko << ", vitajte v obchode. Vas zvysny rozpocet je: " << osoba->rozpocet << endl << "Ponuka sluzieb: \n";
	cout <<"  1. Vyhladavanie produktov podla nazvu \n  2. Vyhladavanie produktov podla vyrobcu \n  3. Ukoncit nakup a opustit obchod \n";

	if (osoba->rozpocet == 0) {				//kontrola vycerpania rozpoctu
		volba = 3;
		cout << "Rozpocet bol vycerpany. Ukoncenie nakupu.\n";
	}

	while (volba < 1 || volba > 3) {		//kontrola vstupu
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Zadajte vasu volbu: ";
		cin >> volba;
	}

	if (volba == 1) {
		predaj_nazov(osoba, produkty);
	}
	else if (volba == 2) {
		predaj_vyrobca(osoba, produkty);
	}
	else if (volba == 3) {
		int zaplatit = 0;
		for (i = 1; i <= 13; i++) {
			zaplatit += (osoba->kosik[i].cena * osoba->kosik[i].sklad);
		}
		cout << "Dakujeme za nakup. Nakupili ste produkty v hodnote " << zaplatit << "\n";
		blocek(osoba, produkty, zaplatit);
		delete[] produkty;
		exit(0);
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx//

int main() {
	ZAKAZNIK* osoba = new ZAKAZNIK;

	PRODUKT* produkty = nacitaj_subor("produkty.txt");
	spoznaj_zakaznika(osoba, produkty);

	predaj(osoba, produkty);
	return 0;
}