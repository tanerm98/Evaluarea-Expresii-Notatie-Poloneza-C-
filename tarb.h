/*--- tarb.h ---*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifndef ARBORE_BINAR
#define ARBORE_BINAR

typedef int TInfo;
typedef struct nod {
	int tip; //1 - operator, 2 - operand, 3 - valoare numerica
	void* info;
	struct nod *st, *dr;
} TNod, *TArb, **AArb;



TArb ConstrFr(void* x, int tip);
     /* -> adresa frunza cu informatia x, sau NULL daca nu exista spatiu */
int NrNoduri(TArb a);     /* numar noduri */
void distruge (TArb *r);
void eroare (char **variabile, int *valori, TArb a, int N);
int calcSDR (TArb *arb);
int gasesteVal (char *token, char **variabile, int *valori, int N, int *ok);
int calculeazaVal (char *token);
int AdaugNod (TArb *arb, TArb nou);

void citireVariabile (char **variabile, int *valori, int N, FILE *in);
void rezolvareEcuatii (FILE *in, FILE *out, char **variabile, int *valori, int N);
#endif

#ifndef _random_
#define _random_
#define random(n) (rand() % (n))
#define randomize() (srand((unsigned int)time(NULL)))
#endif

