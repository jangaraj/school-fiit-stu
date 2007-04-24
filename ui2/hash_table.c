#include <stdio.h>
#include <stdlib.h>
#include "const.h"
#include "hash_table.h"
//#include "stav.h"

HASH_TABLE *vytvor_tabulku(void) {
	HASH_TABLE *p_pom;
	int prvkov_tabulky, i;

	if((p_pom = (HASH_TABLE *) malloc(sizeof(HASH_TABLE))) == NULL  ) {	
		fprintf(stderr,"Nedostatok pamate pri alokacii hashovacej tabulky");
		exit(1);
	}
	prvkov_tabulky = factorial(POCET);
	if((p_pom->table = (int *) malloc(prvkov_tabulky * sizeof(int))) == NULL) {
		printf("mallockujem pole %d\n", prvkov_tabulky);
		fprintf(stderr,"Nedostatok pamate pri alokacii hashopvacej tabulky 2");
		exit (1);
	}
	//vynulovanie tabulky - nic nie je v mnozine
	for(i=0;i<prvkov_tabulky;i++) {
		p_pom->table[i] = 0;
	}
	for (i=0;i<POCET;i++)  p_pom->koef[i] = factorial (POCET - 1 - i);
			p_pom->koef[POCET - 1] = 0;	//pre jednoznacne urcenie stavu nam staci n-1 policok
	return p_pom;
}

//vypocet faktorialu - potrebne pre urcenie velkosti hashovacje tabulky
int factorial(int i) {
	int fact = 1;

	if((i==0) || (i==1)) return 1;
	do {	
		fact *= i;	
	}
	while (--i > 1);
	return fact;
}

//hashovacia funkcia
int hash(stav s, HASH_TABLE *p_hash_table) {
	int i, j;

	//predspracovanie stavu:
	for (i=0; i<POCET - 1; i++)	{
		for (j=0; j<i; j++)	{
			if (s.polia[i] > s.polia[j]) s.polia[i]--;
		}
	}
	//vynasobenie koeficientami:
	int vystup=0;
	for (i=0; i<POCET; i++) vystup += ((s.polia[i]) * (p_hash_table->koef[i]));
	//exit(5);
	return vystup;
}

//pridanie stavu do mnoziny
void hash_insert(stav s, HASH_TABLE *p_hash_table) {
	if(p_hash_table->table[hash(s, p_hash_table)]) printf ("Chyba pri vkladani do hash tabulky - nastala kolizia.\n");
	p_hash_table->table[hash(s, p_hash_table)] = 1;	//nastavenie priznaku v tabulke, ze sa stav nachadza v mnozine
}

//zistenie, ci sa stav s nachadza v mnozine
int hash_is_in(stav s, HASH_TABLE *p_hash_table) {
		return p_hash_table->table[hash(s, p_hash_table)];
}

