#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tarb.h"

void distruge(TArb *r) 	//distruge toate nodurile
{
	if (!(*r)) return;
	distruge (&((*r)->st));
	distruge (&((*r)->dr));
	if ((*r)->info) {
		free ( (*r)->info );
		(*r)->info = NULL;
	}
	free (*r);
	(*r) = NULL;
}

void eroare (char **variabile, int *valori, TArb a, int N)
//functie care elibereaza memoria in caz de eroare de alocare
{
	int i;
	printf ("Eroare la alocare!\n");
	for (i = 0; i <= N; i++) {
		free (variabile[i]);
	}
	free (variabile);
	free (valori);
	distruge (&a);
}

int calcSDR (TArb *arb)
//calculeaza in ordinea SDR arborele si elimina nodurile parcurse
{
	int x;

	if ( ((*arb) == NULL) || ((*arb)->tip >= 2) ) {return 1;} //tip 2 = frunza

	x = calcSDR (&((*arb)->st));
	if (x != 1) {return x;}
	//trebuie sa returneze -1, 0 sau 1, in functie de succesul operatiei
	x = calcSDR (&((*arb)->dr));
	if (x != 1) {return x;}

	char copie[30];
	TArb a = (*arb);
	int *rez;

	if (a->st) {
		if (a->st->tip == 3) {
		//tip 3 = variabila nedeclarata
		//returneaza codul de eroare pt acest caz
			free (a->st->info);
			free (a->st);
			a->st = NULL;
			return -1;
		}
	}

	if (a->dr) {
		if (a->dr->tip == 3) {
		//la fel ca mai sus
			free (a->dr->info);
			free (a->dr);
			a->dr = NULL;
			return -1;
		}
	}
	
	rez = calloc (1, sizeof(int));
	//aici se va retine rezultatul operatiei
	//rez va inlocui un nod ce contine un operand
	
	strcpy(copie, (char*)(a->info)); //aflam ce operator este a

	if (copie[0] == '+') {
		*rez = (*(int*)(a->st->info)) + (*(int*)(a->dr->info));
	}
	else if (copie[0] == '-') {
		*rez = (*(int*)(a->st->info)) - (*(int*)(a->dr->info));
	}
	else if (copie[0] == '*') {
		*rez = (*(int*)(a->st->info)) * (*(int*)(a->dr->info));
	}
	else if (copie[0] == '/') {
		if ((*(int*)(a->dr->info)) == 0) {free (rez); return 0;}
		*rez = (*(int*)(a->st->info)) / (*(int*)(a->dr->info));
	}
	else if (copie[0] == '^') {
		*rez = pow ((*(int*)(a->st->info)),(*(int*)(a->dr->info)));
	}
	else if (!strcmp(copie, "sqrt")) {
		if ( (*(int*)(a->st->info)) < 0 ) {
			free (rez);
			return 0;
		}
		*rez = sqrt (*(int*)(a->st->info));
	}

		//stergem fiii lui a, deja calculati
		free ((*arb)->st->info);
		free ((*arb)->st);
		(*arb)->st = NULL;

		free ((*arb)->dr->info);
		free ((*arb)->dr);
		(*arb)->dr = NULL;

		//updatam pe a
		free ((*arb)->info);
		(*arb)->info = rez;
		(*arb)->tip = 2; //am modificat continutul nodului

		return 1;
	
}

TArb ConstrFr(void* x, int tip)
//construieste o frunza cu info x si tipul tip
{
	TArb aux = (TArb)malloc (sizeof(TNod));
	if (!aux) {return NULL;} //da, imi plac acoladele

	aux->info = x;
	aux->st = aux->dr = NULL;
	aux->tip = tip;

	return aux;
}

int gasesteVal (char *token, char **variabile, int *valori, int N, int *ok)
//afla valoare numerica a unui operand de forma a, b, c etc
{
	int i, p = 0;
	*ok = 0; //0 daca s-a gasit, 1 in caz contrar
	//asta ajuta la a determina daca variabila a fost sau nu declarata

	for (i = 0; i < N; i++) {
		if ( !strcmp(token, variabile[i]) ) {
			p = valori[i];
			*ok = 1;
		}
	}
	return p;
}

int calculeazaVal (char *token)
//calculeaza valoarea unui numar reprezentat printr-un sir de caractere
{
	int suma = 0, len = strlen (token) - 1, i;

	for (i = 0; i <= len; i++) {
		suma = suma * 10 + (token[i] - '0');
	}

	return suma;
}

int AdaugNod (TArb *arb, TArb nou)
//adauga un nod nou arborelui, dupa regula necesara
{
	int x = 0;
	TArb a = *arb, aux;

	if (a == NULL) {
		if (nou->tip == 1) { //nu se adauga nod sub cele de tip 2

			if (strcmp ( ((char*)(nou->info)), "sqrt") == 0) {

			//pt sqrt, se adauga noduri doar in stanga
			//dar adaugam si in dreapta un nod (pe care
			//nu il folosim) pentru a umple spatiul =>
			//o construire mai simpla a arborelui

				int *val;
				val = calloc (1, sizeof(int));
				if (!val) {printf ("Eroare de alocare!\n");}
				*val = 1; //am ales aleator aceasta valoare

				aux = ConstrFr (val, 2);
				if (!aux) {printf ("Eroare de alocare!\n");}
				nou->dr = aux;
			}
		}

		*arb = nou;
		return 1;

	} else {

		if (a->tip > 1) {
		//nu se adauga niciun nod sub un operand
			return 0;
		}

		if (a->st == NULL) {

			//de preferat sa se adauge in cel mai de jos element st
			if (nou->tip == 1) {
				if (strcmp ( ((char*)(nou->info)), "sqrt") == 0) {
					int *val;
					val = calloc (1, sizeof(int));
					if (!val) {printf ("Eroare de alocare!\n");}
					*val = 1; //am ales aleator aceasta valoare

					aux = ConstrFr (val, 2);
					if (!aux) {printf ("Eroare de alocare!\n");}
					nou->dr = aux;
				}
			}

			a->st = nou;
			return 1;

		} else {

			//se parcurge si mai jos arborele, pe partea stanga
			x = AdaugNod (&(a->st), nou);

			if (x == 1) { //adaugarea reuseste
				return 1;

			} else {
				
				if (a->dr == NULL) {

					if ((nou->tip == 1) &&
					(strcmp (((char*)(nou->info)),"sqrt") == 0)) {

						int *val;
						val = calloc (1, sizeof(int));
						if (!val) {printf ("Eroare de alocare!\n");}
						*val = 1; //am ales aleator aceasta valoare

						aux = ConstrFr (val, 2);
						if (!aux) {printf ("Eroare de alocare!\n");}
						nou->dr = aux;
					}

					a->dr = nou;
					return 1;

				}
				//se parcurge si mai jos arborele, pe partea dr
				x = AdaugNod (&(a->dr), nou);

				if (x == 1) { return 1; }
				else { return 0; } //nu s-a reusit adaugarea
			}
		}
	}
}
