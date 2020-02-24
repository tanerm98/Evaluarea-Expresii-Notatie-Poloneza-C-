#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tarb.h"

void citireVariabile (char **variabile, int *valori, int N, FILE *in)
//citeste variabilele si valorile acestora
{
	int i, numar;
	char numev[30], dump[5];

	for (i = 0; i < N; i++) {
	//se citeste numele variabilei, semnul = si apoi valoarea numerica

		fscanf (in, "%s", numev);

		variabile[i] = (char*)
		malloc ( (strlen(numev) + 1 ) * sizeof (char) );
		if (!variabile[i]) {
			eroare (variabile, valori, NULL, N);
			return;
		}

		strcpy (variabile[i], numev);	

		fscanf (in, "%s", dump); //citirea semnului =

		fscanf (in, "%d", &numar);
		valori[i] = numar;		
	}
}

void rezolvareEcuatii (FILE *in, FILE *out, char **variabile, int *valori, int N)
{
	//citire si rezolvare ecuatii

	int M, tip, *val, len, ok, valid = 1, negativ = 0, i;
	TArb a = NULL, nou = NULL;
	char *operand, nedeclarat[30], 
	dump[5], ecuatie[300], copie[30], *token;

	fscanf (in, "%d", &M);
	fgets (dump, 5, in); //trebuie citit endline-ul, ptc vom volosi fgets

	for (i = 1; i <= M; i++) {

		ok = 1; //aici retinem daca variabilele din ecuatie exista

		fgets (ecuatie, 300, in);
		len = strlen (ecuatie);
		if (ecuatie [len - 1] == '\n') {
			ecuatie [len - 1] = '\0';
			//pentru o mai buna token-izare
		}

		token = strtok (ecuatie,  " ");

		while (token != NULL) {

			if (strstr("+-*/^", token) || (!strcmp(token, "sqrt")) ) {
				tip = 1; //operator

			} else if ((token[0] >= 48) && (token[0] <= 57)) {
				//cod ASCII pentru cifra
				tip = 3; //val numerica pozitiva

			} else {
				if ((token[1] >= 48) && (token[1] <= 57)) 
				//se ajunge aici daca token[0] == '-'
				//se verifica daca token[1] este o cifra
				{
					tip = 3; //val numerica negativa
				} else {
					tip = 2; //variabila (ex: a, b, c)
				}
			}

			if (tip == 2) { //variabila

				negativ = 0; //retinem aici daca are '-' in fata

				if (token[0] == '-') {
					negativ = 1;
				}
				if (negativ) {	
					strcpy (copie, token + 1); 
					strcpy(token, copie); //eliminam semnul -
				}

				val = calloc (1, sizeof(int));
				if (!val) {
					eroare (variabile, valori, a, N);
					return;
				}

				*val = gasesteVal (token, variabile, valori, N, &ok);

				if (negativ) {(*val) = (*val) * (-1);}
				negativ = 0;

				if (ok == 0) {
					//chiar daca nu a fost definita variabila
					//se creeaza un nod cu aceasta
					//verificarea se face la evaluarea ecuatiei
					*val = 1; //am ales aleator
					strcpy (nedeclarat, token);
					nou = ConstrFr (val, 3); 
					//tip 3 pt variabila nedefinita

				} else {
					nou = ConstrFr (val, 2);
				}

				if (!nou) {
					eroare (variabile, valori, a, N);
					return;
				}

				AdaugNod (&a, nou);

			} else if (tip == 3) {

				negativ = 0;

				if ( strchr (token, '-') ) {
					negativ = 1;
				}
				if (negativ) {
					strcpy (copie, token + 1);
					strcpy(token, copie);
				}

				val = calloc (1, sizeof(int));
				if (!val) {
					eroare (variabile, valori, a, N);
					return;
				}

				*val = calculeazaVal (token);

				if (negativ) { (*val) = (*val) * (-1); }
				negativ = 0;

				nou = ConstrFr (val, 2);
				if (!nou) {
					eroare (variabile, valori, a, N);
					return;
				}

				AdaugNod (&a, nou);

			} else {
				//tipul este operand (ex: +, -, ^, sqrt)
				len = strlen (token) + 1;

				operand = (char*) calloc (len, sizeof (char));
				if (!operand) {
					eroare (variabile, valori, a, N);
					return;
				}

				strcpy (operand, token);

				nou = ConstrFr (operand, 1);
				if (!nou) {
					eroare (variabile, valori, a, N);
					return;
				}

				AdaugNod (&a, nou);

			}

			token = strtok (NULL, " ");
		}

		valid = calcSDR (&a);
		//0  daca e operatie nevalida, -1 daca e un operand nedeclarat
		//si 1 invers

		if (valid == 0) {
			fprintf (out, "Invalid expression\n");

		} else if (valid == -1) {
			fprintf (out, "Variable %s undeclared\n", nedeclarat);

		} else {
			fprintf (out, "%d\n", *(int*)(a->info));
		}

		distruge (&a);
		a = NULL;

	}
}

int main (int argc, char**argv)
//aici se deschid fisierele de input/output si se apeleaza functiile de
//citire si rezolvare a ecuatiilor
{
	int i, *valori, N;
	char **variabile = NULL;

	FILE *in = fopen (argv[1], "r");
	if (!in) {
		printf ("Eroare la deschiderea fisierului de input!\n");
	}
	
	FILE *out = fopen (argv[2], "w");
	if (!out) {
		printf ("Eroare la deschiderea fisierului de output!\n");
	}
	fscanf (in, "%d", &N);

	//memoram numele si valorile variabilelor in matrici si vectori
	variabile = (char**) calloc ((N + 1), sizeof(char*)); //a, b, c
	if (!variabile) {
		printf ("Eroare la alocare!\n");
		fclose (in);
		fclose (out);
		return 0;
	}

	valori = calloc (N, sizeof (int)); //val numerica
	if (!valori) {
		printf ("Eroare la alocare!\n");
		for (i = 0; i <= N; i++) {
			free (variabile[i]);
		}
		free (variabile);
		fclose (in);
		fclose (out);
		return 0;
	}

	citireVariabile (variabile, valori, N, in);

	rezolvareEcuatii (in, out, variabile, valori, N);

	//eliberam restul memoriei ocupate
	for (i = 0; i <= N; i++) {
		free (variabile[i]);
	}
	free (variabile);
	free (valori);
	

	fclose (in);
	fclose (out);

	return 0;
}
