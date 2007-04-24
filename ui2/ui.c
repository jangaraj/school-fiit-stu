/*
 * Jan Garaj, 24081, FIIT STU
 * zadanie c.2 UI - 8 hlavolam - obosjmerne prehladavanie
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "split_stdin.h"						//naciatavanie interaktivneho vstupu
#include "stav.h"
#include "const.h"
#include "hash_table.h"
#include "front.h"
#include "prvok_strom.h"

#define OP_DOWN 'D'
#define OP_LEFT 'L'
#define OP_RIGHT 'R'
#define OP_UP 'U'

int nacitaj_data(stav *stav);
int vypis_stav(stav *stav, char sprava[255]);  
char invert_op(char op);
int najdi_medzeru(stav s);
void pridaj_nasled(prvok_strom *vrchol, prvok_strom *nasled);
prvok_strom * najdi_strom(prvok_strom *koren, stav s, char * operatory);
stav *right(stav s);
stav *left(stav s);
stav *up(stav s);
stav *down(stav s);

int main(int argc, char *argv[]) 
{
    int o, mode, dlzka, debug;
	float time;
	stav *start_stav, *end_stav, priesecnik, *pom_stav;
	long spracovanych=0;	//pocet spracovanych stavov
	long vytvorenych=2;		//pocet vytvorenych stavov (pociatocny a koncovy uz boli vytvorene)
	int najdene=0;
	char *historia2 = NULL;		//historia operatorov v 2. hladani pre tento stav
	prvok_strom *pom_s, *pom_s2;
	stav_s_historiou *pom_h, *pom_h2;
	struct timeval a, b;
    opterr = 0;
	mode = 0;    //mod programu 0 - demodata, 1 - interactivny vstup
	debug = 0;
    
	while ((o = getopt(argc, argv, "hiv")) != -1)	//spracovanie parametrov programu
            switch (o) 
            {
                case 'h':
					printf("Jan Garaj, 24081, FIIT\n");
	                printf("Umela Inteligencia - zadanie cislo 2 - hlavolam 8: obojsmerne prehladavanie\n");
					printf("parameter -h pre tento help\n");
					printf("parameter -i pre interaktivne nacitanie vstupu, format vstupu 1 0 8 7 6 5 4 3 2\n");
					printf("parameter -v pre dubug vypisy\n");
					printf("bez parametrov - pouziju sa demodata zabetonovane v binarke\n");
					exit(0);
		    	    break;
				case 'i':
					mode = 1;
					break;
				case 'v':
					debug = 1;
					break; 
                default:
                    printf("'%c' je nespravny prepinac", optopt);
                    printf(" alebo ma nespravny argument\n");
					printf("Pouzi prepinac -h pre help\n");
					exit(1);
	    	} // koniec: switch aj while
	argc -= optind;
   	argv += optind;

	//alokacia pamate pre stavy
	if((start_stav=(stav *) malloc(sizeof(stav)))==NULL) {
		fprintf(stderr,"Malo pamate na alokaciu start stavu\n");
		exit (1);
	}
 	if((end_stav=(stav *) malloc(sizeof(stav)))==NULL) {
		fprintf(stderr,"Malo pamate na alokaciu end stavu\n");
		exit (1);
	}
	if(mode) {
		//nacitanie interactivnych dat
		if(nacitaj_data(start_stav)) return 1;
		vypis_stav(start_stav,"Vstupny stav, ktory bol interaktivne nacitany");
	}
	else {
		//nacitanie demodat vstupneho stavu
		start_stav->polia[0] = 0;
		start_stav->polia[1] = 1;
		start_stav->polia[2] = 2;
		start_stav->polia[3] = 3;
		start_stav->polia[4] = 4;
		start_stav->polia[5] = 5;
		start_stav->polia[6] = 6;
		start_stav->polia[7] = 7;
		start_stav->polia[8] = 8;
		vypis_stav(start_stav,"Vstupny stav");	
	}
	end_stav->polia[0] = 1 ; 
	end_stav->polia[1] = 2;
	end_stav->polia[2] = 3;
	end_stav->polia[3] = 4;
	end_stav->polia[4] = 5;
	end_stav->polia[5] = 6;
	end_stav->polia[6] = 7;
	end_stav->polia[7] = 0;
	end_stav->polia[8] = 8;
	vypis_stav(end_stav,"Pozadovany koncovy stav");	
	//kontrola ci sa koncovy stav nezhoduje so zaciatocnym
	if((start_stav->polia) == (end_stav->polia)) {
		printf ("Pociatocny stav sa zhoduje s koncovym, nie je co riesit :P\n");
		exit(1);
	}
	//hash1 pre hladanie 1.smerom
	HASH_TABLE *hash1;
	hash1 = vytvor_tabulku();
	//hash2 pre hladanie 2.smerom
	HASH_TABLE *hash2;
	hash2 = vytvor_tabulku();
	//front1 pre hladanie 1.smerom
	front *front1;
	if((front1=(front *) malloc(sizeof(front)))==NULL) {
		fprintf(stderr,"Nedostatok pamate pri vutvarani frontu cislo 1\n");
		exit (1);
	}
	front1->in = front1->out = NULL;
	//front2 pre hladanie 1.smerom
	front *front2;
	if((front2=(front *) malloc(sizeof(front)))==NULL) {
		fprintf(stderr,"Nedostatok pamate pri vutvarani frontu cislo 2\n");
		exit (1);
	}
	front2->in = front2->out = NULL;
	//inicializacia 1. hladania
	prvok_strom *koren;
	if((koren=(prvok_strom *) malloc(sizeof(prvok_strom)))==NULL) {
		fprintf(stderr,"Nedostatok pamate pri vytvarani korena pre 1. hladanie\n");
		exit(1);
	}
	koren->child1 = koren->child2 = koren->child3 = koren->child4 = NULL;
	koren->sta = *end_stav;
	front_insert(koren, front1);
	hash_insert(*end_stav, hash1);
	//inicializacia 2. hladania
	stav_s_historiou *first;
	if((first=(stav_s_historiou *) malloc(sizeof(stav_s_historiou)))==NULL) {
		fprintf(stderr,"Nedostatok pamate pri vytvarani korena pre stav s historiou\n");
		exit(1);
	}
	first->s = *start_stav;
	char *operatory_pole;
	if((operatory_pole=(char *) malloc(sizeof(char)))==NULL) {
		fprintf(stderr,"Nedostatok pamate pri vytvarani pola operatorov\n");
		exit (1);
	}
	first->operatory = operatory_pole;
	first->operatory[0] = '\0';
	front_insert2(first, front2);
	hash_insert(*start_stav, hash2);
	if((pom_h=(stav_s_historiou *) malloc(sizeof(stav_s_historiou)))==NULL) {
		fprintf(stderr,"Nedostatok pamate pri alokovani pomocnej\n");
		exit (1);
	}
	//prehladavaci cyklus
	gettimeofday(&a,NULL);
	while (!(front_is_empty(front1) || front_is_empty(front2))) {
		//vyberieme nespracovany prvok zo zasobnika 2 hladania
		pom_h = front_get2(front2);
		dlzka = strlen(pom_h->operatory);	//dlzka retazca s operatormi
		//printf("dlzka operatorov now: %d\n",dlzka);
		spracovanych++;
		//ak sa stav uz vyskytol v prvom hladani, uspesne skoncime a ulozime stav a historiu operatorov
		if(hash_is_in(pom_h->s, hash1)){
			najdene = 1;
			priesecnik = pom_h->s;	//stav, v ktorom sa hladania "stretli"
			//cesta v 2. hladani (operatory)
			if((historia2=(char *) malloc((dlzka+1)*sizeof(char)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu novej postupnosti operatorov\n");
				exit (1);
			}
			strcpy(historia2,pom_h->operatory);
			//komiec prehladavacieho cyklu
			break;
		}
		if((pom_stav=(stav *) malloc(sizeof(stav)))==NULL) {
			fprintf(stderr,"Nedostatok pamate pri alokacii pomocneho stavu\n");
			exit (1);
		}
		//skusim posunut dolu
		if(debug) printf("------------------------------------------------------------------------------------------\n");
		if(debug) vypis_stav(&pom_h->s, "\nRozvijany stav pred DOWN");
		pom_stav=down(pom_h->s);
		if ((pom_stav != NULL) && !(hash_is_in(*pom_stav, hash2))) {
			if((pom_h2=(stav_s_historiou *) malloc(sizeof(stav_s_historiou)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pri alokacii pomocneho stavu\n");
				exit (1);
			}
			pom_h2->s = *pom_stav;
			//pridanie operatora do historie
			char *historiad;
            if((historiad=(char *) malloc((dlzka+2)*sizeof(char)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu novej postupnosti operatorov\n");
			    exit (1);
			}
			pom_h2->operatory = historiad;
			strcpy(pom_h2->operatory,pom_h->operatory);
			(pom_h2->operatory)[dlzka] = OP_DOWN;
			pom_h2->operatory[dlzka+1] = '\0';
			front_insert2(pom_h2, front2);	//vlozime novy stav s historiou do zasobnika nespracovanych
			hash_insert(*pom_stav, hash2);	//vlozime novy stav medzi uz nadobudnute stavy do hashu
			if(debug) {
				//vypis_stav(&pom_h->s, "2 hladanie stav pred aplikaciou operatoru:");
				vypis_stav(pom_stav, "2 hladanie operator down:");
			}
			free((void *) pom_stav);	//uvolnenie pamate
			vytvorenych++;
		}
		if(debug) vypis_stav(&pom_h->s, "\nRozvijany stav pred LEFT");
		pom_stav=left(pom_h->s);
		if ((pom_stav != NULL) && !(hash_is_in(*pom_stav, hash2))) {
			stav_s_historiou *pom_h2;
			if((pom_h2=(stav_s_historiou *) malloc(sizeof(stav_s_historiou)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pri vytvarani korena pre stav s historiou\n");
				exit(1);
			}
			pom_h2->s = *pom_stav;
			//pridanie operatora do historie:
			char *historial;
            if((historial=(char *) malloc((dlzka+2)*sizeof(char)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu novej postupnosti operatorov\n");
			    exit (1);
			}
			pom_h2->operatory = historial;
			strcpy(pom_h2->operatory,pom_h->operatory);
			pom_h2->operatory[dlzka] = OP_LEFT;
			pom_h2->operatory[dlzka+1] = '\0';
			front_insert2(pom_h2, front2);	//vlozime novy stav s historiou do zasobnika nespracovanych
			hash_insert(*pom_stav, hash2);	//vlozime novy stav medzi uz nadobudnute stavy do hashu
			if(debug) {
//				vypis_stav(&pom_h->s, "2 hladanie stav pred aplikaciou operatoru:");
				vypis_stav(pom_stav, "2 hladanie operator left:");
			}
			free((void *)pom_stav);	//uvolnenie pamate
			vytvorenych++;
		}
		//ak sa da pouzit operator "right" a stav este neexisuje, vytvorime novy uzol
		if(debug) vypis_stav(&pom_h->s, "\nRozvijany stav pred RIGHT");
		pom_stav=right(pom_h->s);
		if ((pom_stav != NULL) && !(hash_is_in(*pom_stav, hash2))) {
			stav_s_historiou *pom_h2;
			if((pom_h2=(stav_s_historiou *) malloc(sizeof(stav_s_historiou)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pri vytvarani korena pre stav s historiou\n");
				exit(1);
			}
			pom_h2->s = *pom_stav;
			//pridanie operatora do historie:
			char *historiar;
            if((historiar=(char *) malloc((dlzka+2)*sizeof(char)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu novej postupnosti operatorov\n");
			    exit (1);
			}
			pom_h2->operatory = historiar;
			strcpy(pom_h2->operatory,pom_h->operatory);
			pom_h2->operatory[dlzka] = OP_RIGHT;
			pom_h2->operatory[dlzka+1] = '\0';
			front_insert2(pom_h2, front2);	//vlozime novy stav s historiou do zasobnika nespracovanych
			hash_insert(*pom_stav, hash2);	//vlozime novy stav medzi uz nadobudnute stavy do hashua
			if(debug) vypis_stav(pom_stav, "2 hladanie operator right:");
			free((void *) pom_stav);	//uvolnenie pamate
			vytvorenych++;
		}
		//ak sa da pouzit operator "up" a stav este neexisuje, vytvorime novy uzol
		if(debug) vypis_stav(&pom_h->s,"\nRozvijany stav pred UP");
		pom_stav=up(pom_h->s);
		if ((pom_stav != NULL) && !(hash_is_in(*pom_stav, hash2))) {
			stav_s_historiou *pom_h2;
			if((pom_h2=(stav_s_historiou *) malloc(sizeof(stav_s_historiou)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pri vytvarani korena pre stav s historiou\n");
				exit(1);
			}
			pom_h2->s = *pom_stav;
			//pridanie operatora do historie:
			char *historiau;
            if((historiau=(char *) malloc((dlzka+2)*sizeof(char)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu novej postupnosti operatorov\n");
			    exit (1);
			}
			pom_h2->operatory = historiau;
			strcpy(pom_h2->operatory,pom_h->operatory);
			pom_h2->operatory[dlzka] = OP_UP;
			pom_h2->operatory[dlzka+1] = '\0';
			front_insert2(pom_h2, front2);	//vlozime smernik na novy stav s historiou do zasobnika nespracovanych
			hash_insert(*pom_stav, hash2);	//vlozime novy stav medzi uz nadobudnute stavy do hashu
			if(debug) vypis_stav(pom_stav,"2 hladanie operator up:");
			free((void *) pom_stav);	//uvolnenie pamate
			vytvorenych++;
		}
		//koniec druheho hladania
		//-----------------------------------------------------------------------------------------------------
		//zaciatok prveho hladania
		//vyberieme nespracovany prvok zo zasobnika prveho hladania
		pom_s = front_get(front1);
		spracovanych++;
		//ak sa da pouzit operator "down" a stav este neexistuje, vytvorime novy uzol
		pom_stav=down(pom_s->sta);
		if ((pom_stav != NULL) && !(hash_is_in(*pom_stav,hash1))) {
			if((pom_s2=(prvok_strom *) malloc(sizeof(prvok_strom)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu pomocneho prvku stromu\n");
				exit (1);
			}
			pom_s2->sta = *pom_stav;
			pom_s2->oper = OP_DOWN;
			pom_s2->child1 = pom_s2->child2 = pom_s2->child3 = pom_s2->child4 = 0;
			pridaj_nasled(pom_s, pom_s2);
			front_insert(pom_s2, front1);	//vlozime smernik na novy vrchol stromu do zasobnika nespracovanych
			hash_insert(*pom_stav, hash1);	//vlozime novy stav medzi uz nadobudnute stavy do hashu
			free((void *) pom_stav);	//uvolnenie pamate
			vytvorenych++;
		}

		//ak sa da pouzit operator "left" a stav este neexistuje, vytvorime novy uzol
		pom_stav=left(pom_s->sta);
		if ((pom_stav != NULL) && !(hash_is_in(*pom_stav, hash1))) {
			if((pom_s2=(prvok_strom *) malloc(sizeof(prvok_strom)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu pomocneho prvku stromu\n");
				exit (1);
			}
			pom_s2->sta = *pom_stav;
			pom_s2->oper = OP_LEFT;
			pom_s2->child1 = pom_s2->child2 = pom_s2->child3 = pom_s2->child4 = 0;
			pridaj_nasled(pom_s, pom_s2);
			front_insert(pom_s2, front1);	//vlozime smernik na novy vrchol stromu do zasobnika nespracovanych
			hash_insert(*pom_stav, hash1);	//vlozime novy stav medzi uz nadobudnute stavy do hashu
			free((void *) pom_stav);	//uvolnenie pamate
			vytvorenych++;
		}

		//ak sa da pouzit operator "right" a stav este neexistuje, vytvorime novy uzol
		pom_stav=right(pom_s->sta);
		if ((pom_stav != NULL) && !(hash_is_in(*pom_stav, hash1))) {
			if((pom_s2=(prvok_strom *) malloc(sizeof(prvok_strom)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu pomocneho prvku stromu\n");
				exit (1);
			}
			pom_s2->sta = *pom_stav;
			pom_s2->oper = OP_RIGHT;
			pom_s2->child1 = pom_s2->child2 = pom_s2->child3 = pom_s2->child4 = 0;
			pridaj_nasled(pom_s,pom_s2);
			front_insert(pom_s2, front1);	//vlozime smernik na novy vrchol stromu do zasobnika nespracovanych
			hash_insert(*pom_stav, hash1);	//vlozime novy stav medzi uz nadobudnute stavy do hashu
			free((void *) pom_stav);	//uvolnenie pamate
			vytvorenych++;
		}

		//ak sa da pouzit operator "up" a stav este neexistuje, vytvorime novy uzol
		pom_stav=up(pom_s->sta);
		if ((pom_stav != NULL) && !(hash_is_in(*pom_stav, hash1))) {
			if((pom_s2=(prvok_strom *) malloc(sizeof(prvok_strom)))==NULL) {
				fprintf(stderr,"Nedostatok pamate pre alokaciu pomocneho prvku stromu\n");
				exit (1);
			}
			pom_s2->sta = *pom_stav;
			pom_s2->oper = OP_UP;
			pom_s2->child1 = pom_s2->child2 = pom_s2->child3 = pom_s2->child4 = 0;
			pridaj_nasled(pom_s,pom_s2);
			front_insert(pom_s2, front1);	//vlozime smernik na novy vrchol stromu do zasobnika nespracovanych
			hash_insert(*pom_stav, hash1);	//vlozime novy stav medzi uz nadobudnute stavy do hashu
			free((void *) pom_stav);	//uvolnenie pamate
			vytvorenych++;
		}


	}//while prehladavacieho alg
	gettimeofday(&b,NULL);
	time = (b.tv_sec-a.tv_sec) * 1000000 + (b.tv_usec-a.tv_usec);
	//time = b.tv_usec - a.tv_usec;
	printf("\n*************************************************\n");
	printf("Trvanie hlavnej prehladavacej slucky: %f sekund\n", time/1000);
	printf("Prehladavaci cyklus prebehol s nasledovnymi parametrami:\n%ld uzlov vytvorenych\n%ld uzlov spracovanych\n",vytvorenych, spracovanych);
	if(najdene) {
		char buffer[10000];
		vypis_stav(&priesecnik,"\nPrehladavania sa stretli v stave:");
		//je nutne najst postupnost operatorov, ktorymi sme sa v 1. hladani dostali k "priesecniku"
		(void *) najdi_strom(koren,priesecnik,buffer);
		char *cesta;
		if((cesta = (char *) malloc((strlen(buffer) + strlen(historia2) + 1)*sizeof(char)))==NULL) {
			fprintf(stderr,"Nedostatok pamate pre cestu vysledku\n");
			exit (1);
		}
		strcpy(cesta, historia2);
		strcat(cesta, buffer);
		printf("Postupnost je: %s\n", cesta);
		printf("Pocet krokov od zaciatku k cielu = %d\n",strlen(cesta));
		
	} else printf("Riesenie nebolo najdene\n");





	free((void *) hash1);
	free((void *) hash2);
	free((void *) front1);
	free((void *) front2);
	return 0;
}

//funkcia na vypis stavu hlavolamu
int vypis_stav(stav *stav, char sprava[255]) {
	int i, j;

	printf("%s\n",sprava);
	for(i=0;i<HRANICA;i++) {
	  for(j=0;j<HRANICA;j++) {
		  printf("%d ",stav->polia[(i*HRANICA)+j]);
	  }
	  printf("\n");
	}
	return 0;
}

//funkcia zabezpucujuca interaktivne nacitanie vstupneho stavu
int nacitaj_data(stav *stav) {
	int i, j, fields, *pocet;
	char **response;
	
	if((pocet = malloc(sizeof(int)))==NULL) {
		fprintf(stderr,"Malo pamate na alokaciu premennej pocet\n");
		exit (1);
	}
	response = split_stdin(" ",&fields, pocet);
	if((*pocet)!=POCET) {
		fprintf(stderr,"Chybny pocet vstupnych dat\n");
		exit (1);
	}
	for(i=0;i<HRANICA;i++) {
		for(j=0;j<HRANICA;j++) {
			stav->polia[(i*HRANICA)+j] = atoi(response[(i*HRANICA)+j]);			
		}
	}
	free_split_stdin(response, fields);
	return 0;
}

//funkcia vracajuca inverzny operator
char invert_op(char op) {
	char op_i;

	switch(op) 	{
		case OP_DOWN: 
			op_i = OP_UP; 
			break;
	  	case OP_LEFT:
			op_i = OP_RIGHT;
			break;
		case OP_RIGHT:
			op_i = OP_LEFT;
			break;
	   	case OP_UP:
			op_i = OP_DOWN;
			break;
		default:
			op_i = op;
	}
	return op_i;
}

//funkcia na zistenie polohy policka s medzerou
int najdi_medzeru(stav s) {
	int i=0;
	while (s.polia[i] != 0) i++;
	return i;
}

//operatory
stav *right(stav s) {
	int medzera = najdi_medzeru(s);
	//printf("medzera right na %d\n", medzera);
	//printf("medzera:%d % HRANICA:%d je %d\n",medzera, HRANICA,(medzera % HRANICA));
	if (((medzera)%(HRANICA)) == 0) return NULL;	//ak sa medzera nachadza na lavom kraji, operator nemozno pouzit
	stav *pom;
	if((pom = (stav *) malloc(sizeof(stav)))==NULL) { printf("Nedostatok pamate pri operacii right.\n"); exit(1); };
	*pom=s;	//prekopirovanie stareho stavu
	//"posunutie" policka
	//printf("vymena right 0 s %d\n",pom->polia[medzera-1]);
	pom->polia[medzera] = pom->polia[medzera-1];
	pom->polia[medzera-1] = 0;
	return pom;
}

//Posunutie policka nalavo od medzery do medzery
stav *left(stav s) {
	int medzera = najdi_medzeru(s);
	//printf("leftujem, medzera:%d HRANICA:%d modulo:%d\n", medzera+1, HRANICA, ((medzera+1)%(HRANICA)));
	if (((medzera+1)%(HRANICA)) == 0) return NULL;	//ak sa medzera nachadza na pravom kraji, operator nemozno pouzit
	stav *pom;
	if((pom = (stav *) malloc(sizeof(stav)))==NULL) { printf("Nedostatok pamate pri operacii left.\n"); exit(1); };
	*pom=s;	//prekopirovanie stareho stavu
	//"posunutie" policka
	//printf("0 davam za %d\n",pom->polia[medzera+1]);
	pom->polia[medzera] = pom->polia[medzera+1];
	pom->polia[medzera+1] = 0;
	return pom;
}

//Posunutie policka pod medzerou do medzery
stav *up(stav s) {
	int medzera = najdi_medzeru(s);
	if (((medzera)+(HRANICA)) >= POCET) return NULL;	//ak pod medzerou nie je ziaden prvok, operator nemozno pouzit
	stav *pom;
	if((pom = (stav *) malloc(sizeof(stav)))==NULL) { printf("Nedostatok pamate pri operacii up.\n"); exit(1); };
	*pom=s;	//prekopirovanie stareho stavu
	//"posunutie" policka
	pom->polia[medzera] = pom->polia[medzera+HRANICA];
	pom->polia[medzera+HRANICA] = 0;
	return pom;
}

//Posunutie policka nad medzerou do medzery
stav *down(stav s) {
	int medzera = najdi_medzeru(s);
	if (((medzera)-(HRANICA)) < 0) return NULL;	//ak nad medzerou nie je ziaden prvok, operator nemozno pouzit
	stav *pom;
	if((pom = (stav *) malloc(sizeof(stav)))==NULL) { printf("Nedostatok pamate pri operacii down.\n"); exit(1); };
	*pom=s;	//prekopirovanie stareho stavu
	//"posunutie" policka
	pom->polia[medzera] = pom->polia[medzera-HRANICA];
	pom->polia[medzera-HRANICA] = 0;
	return pom;
}

//Pridanie nasledovnika vrcholu stromu
void pridaj_nasled(prvok_strom *vrchol, prvok_strom *nasled) {
	if(vrchol->child1==NULL) vrchol->child1=nasled;
	else if(vrchol->child2==NULL) vrchol->child2=nasled;
	else if(vrchol->child3==NULL) vrchol->child3=nasled;
	else if(vrchol->child4==NULL) vrchol->child4=nasled;
}

//Najdenie vrcholu stromu obsahujuci dany stav
prvok_strom *najdi_strom(prvok_strom *koren, stav s, char *operatory) {
	if((koren)==(NULL)) {
		//printf("mam null\n");
		return NULL;
	}
	//if(koren->sta==stav.s)


	prvok_strom *pom;
	int dlzka=0;
	char inv_operator = invert_op(koren->oper);
		pom = najdi_strom(koren->child1,s,operatory);
	if(pom != NULL) { 
		dlzka=strlen(operatory);
		//printf("child1\n");
		operatory[dlzka] = inv_operator;
		operatory[dlzka+1] = '\0';
		//dlzka++;
		return pom;
	}
	pom = najdi_strom(koren->child2,s,operatory);
	if(pom != NULL) { 
		dlzka=strlen(operatory);
		//printf("child2\n");
		operatory[dlzka] = inv_operator;
		operatory[dlzka+1] = '\0';
		//dlzka++;
		return pom;
	  }
	if(koren!=NULL) {
		dlzka=strlen(operatory);
		//printf("child4\n");
		operatory[dlzka] = inv_operator;
		operatory[dlzka+1] = '\0';
		//dlzka++;
		pom = najdi_strom(koren->child4,s,operatory);
		return pom;
	}
	pom = najdi_strom(koren->child3,s,operatory);
	if(pom != NULL) { 
		dlzka=strlen(operatory);
		//printf("child3\n");
		operatory[dlzka] = inv_operator;
		operatory[dlzka+1] = '\0';
		//dlzka++;
		return pom;
	}
	pom = najdi_strom(koren->child4,s,operatory);
	if(pom != NULL) { 
		dlzka=strlen(operatory);
		//printf("child4\n");
		operatory[dlzka] = inv_operator;
		operatory[dlzka+1] = '\0';
		//dlzka++;
		return pom;
	}

	if(koren->sta.polia == s.polia) { 
		operatory[0] = inv_operator;
		operatory[1] = '\0';
		return koren;
	}

	operatory[0] = 'N';
	operatory[1] = '\0';
	return NULL;
}
