/*
 * Jan Garaj, 24081, FIIT STU
 * zadanie c.1 TEAP - knihovnik
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int shiftuj(int *pole, int miest, int start, int end);
int zotried(int *pole, int start, int end); 

int pocet; //globalna premenna aby som ju videl aj pri zoradovani a mohol podla nej zoradovat

int main(int argc, char *argv[]) 
{
    int o, i, p, kresli, mode, pomocna, *knihy;
    char *filename;				/* nazov subor ktory obsahuje vstupne data */
	FILE *fr;					/* file handler inupt suboru */
    opterr = 0; mode = 0; kresli =0;
    
	while ((o = getopt(argc, argv, "hf:")) != -1)	//spracovanie parametrov programu
            switch (o) 
            {
                case 'h':
					printf("Jan Garaj, 24081, FIIT\n");
	                printf("Zadanie 1342 - TEAP\n");
					printf("parameter -h pre tento help\n");
					printf("paramater -f filename pre nacitanie dat zo suboru s menom filename\n");
					printf("bez parametrov - pouziju sa demodata zabetonovane v binarke\n");
					exit(0);
		    	    break;
				case 'f':
					mode = 1;
					if((filename=(char *) malloc(strlen(optarg)))==NULL) {
						fprintf(stderr,"Malo pamate na alokaciu nazvu otvaraneho suboru\n");
						exit(1);
					}
					strcpy(filename,optarg);
		            break;
                default:
                        printf("'%c' je nespravny prepinac", optopt);
                        printf(" alebo ma nespravny argument\n");
						printf("Pouzi prepinac -h pre help\n");
						exit(1);
	    	} // koniec: switch aj while
	argc -= optind;
   	argv += optind;
	if(mode==1)	{
		//nacitanie dat zo suboru
		printf("Program vo file mode - data nacitane z %s\n",filename);
		if((fr=fopen(filename, "r"))==NULL) {
			fprintf(stderr,"Chyba otvaranie suboru.\n");
			exit(1);
		}
		//nacitanie prveho riadku suboru = pocet prvkov pola
		if((fscanf(fr, "%d", &pocet))==-1) {
			fprintf(stderr,"Chyba pri nacitani poctu vstupnych dat zo suboru\n");
			exit(1);
		}
		//prvy riadok urcuje kolko prvkove pole je vstupom
		if(pocet<0) {
			fprintf(stderr,"Nulovy resp. zaporny pocet vstupnych dat podla prveho riadku vo vstupnom subore\n");
			exit(1);
		}
		printf("Pocet knih v poli: %d\n",pocet);
		//alokacia pamati pre pole zo suboru
		if((knihy=malloc(pocet*sizeof(int)))==NULL) {
			fprintf(stderr,"Malo pamate na alokaciu vstupnych demodat\n");
			exit(1);
		}
		//nacitanie pola zo suboru - jeden riadok = jedna polozka pola
		for(i=0;i<pocet;i++) {
			if((fscanf(fr, "%d", &pomocna))==-1) {
				fprintf(stderr,"Chyba pri nacitani riadku vstupnych dat do pola\n");
				exit(1);
			}
			*(knihy+i) = pomocna;
		//printf("Do pola knihy[%d] pridana hodnota %d\n", i, *(knihy+i));
		}
	}
	else {		
		//pevne zabetonovane demodata v programe
		kresli=1;
		pocet=10;
		if((knihy=(int *) malloc(pocet*sizeof(int)))==NULL) {
			fprintf(stderr,"Malo pamate na alokaciu vstupnych demodat\n");
			exit(1);
		}
		*(knihy+0) = 1;
		*(knihy+1) = 3;
		*(knihy+2) = 5;
		*(knihy+3) = 7;
		*(knihy+4) = 9;
		*(knihy+5) = 2;
		*(knihy+6) = 4;
		*(knihy+7) = 6;
		*(knihy+8) = 8;
		*(knihy+9) = 10;
		printf("Program v demomode - nacitane demodatai:\n");
		//vypis usporiadavaneho pola
		for(i=0;i<pocet;i++) {
			printf("%d ", *(knihy+i));
		}
		printf("\n");		
	}
	//	vlastna praca algoritmu
	kresli=1; 
	p=((pocet/2)/2);
	if(kresli==1) {
		//data v strede ktore sa budu presuvat, cervene sa budu shiftovat
		printf("Nasledovne vyznacene subpole budem posuvat doprava o %d miest:\n",p);
		for(i=0;i<pocet;i++) {
			if(((i+1)>p) && (i<((pocet/2)+p))) printf("%c[1;31m%d ",27, *(knihy+i));
			else printf("%c[0;37;40m%d ",27,*(knihy+i));
		}
		printf("\n");
	}
	//volanie rekurzivnej funkcie zotried nad celym vstupnym polom
	zotried(knihy, 0, pocet-1);
	free((void *) knihy);
	knihy = NULL;
	if(mode==1) if(fclose(fr)) fprintf(stderr,"Chyba zatvarania vstupneho suboru\n");
    return 0;
}

//  zotried(pointer na zotriedovane pole, zaciatok subpola na zoradenie, koniec pola na zoradenie)
int zotried(int *pole, int start, int end) {
	int i, p, length, n, start_shift, end_shift;

	/*for(i=0; i<pocet i++) {
		printf("%d ",*(pole+i));
	}
	printf("\n");*/

	//printf("volam zotried s tymto startom: %d a endom: %d\n",start,end);
	//getchar();
	//vypocet parametrov pre funkciu shiftuj
	length = (end-start+1);
	//podmienka ukoncujuca rekurziu
	if((end-start+1)<3) return 0;
	n = length/2;
	p = n/2;
	start_shift = start+p;
	end_shift = start+n+p-1;
	//printf("start: %d\nend: %d\nn: %d\np: %d\nstart_shift: %d\nend_shift: %d\n",start, end, n, p, start_shift, end_shift);
	//getchar();
	shiftuj(pole, p, start_shift, end_shift);
	printf("Po rotovani zmena na vyznacenych poziciach:\n");
	for(i=0;i<pocet;i++) {
			if(((i+1)>start_shift) && (i<end_shift+1)) printf("%c[1;31m%d ",27, *(pole+i));
			else printf("%c[0;37;40m%d ",27,*(pole+i));
	}
	printf("\n");
	//return 0;
	//rekurzivne volanie zotried nad dvoma uz ciastocne utriedenmi polami
	if(n%2 != 0) {
		zotried(pole, start, start+n-2);
		zotried(pole, start+n-1, start+2*n-1);
	} else {
		zotried(pole, start, start+n-1);
		zotried(pole, start+n, start+2*n-1);

	}
	return 0;
}

//  shiftuj(pointer na shiftovane pole, kolko miest posuvat, zaciatok shiftovaneho subpola, koniec shiftovaneho subpola)
int shiftuj(int *pole, int miest, int start, int end) {
	int pom, velkost, smer=0 ;
	int mensie, i;
	//printf("shift -----\nmiest: %d\nstart: %d\nend: %d\n-----\n",miest,start,end);
	//koniec otacania ak chcem otacat jednoprvkove pole
	if (start==end || !miest) return 0;
	//velkost shiftovaneho pola
	velkost = end-start+1;
	//shiftovanie pola dolava
	if (miest<0) { 
		miest = velkost + miest % velkost; 
		smer = 1; // zmena smeru na dolava
	}
	else miest = miest % velkost; //shiftovanie doprava
	// pre menej vymen staci tocit o mensie cislo z k a v-k
	mensie = (miest < (velkost-miest)) ? miest : (velkost-miest); 	
	//indikuje otocenie doprava
	if (smer && mensie == miest) smer=0;
	for (i=start; i<start+mensie; i++) {
		pom = *(pole+i);
		*(pole+i) = *(pole+velkost-mensie+i);
		*(pole+velkost-mensie+i)= pom;
	}
	if (miest==mensie && !smer) shiftuj(pole, mensie, start+mensie, end); // ak je smer nastaveny doprava a zaroven sa posuvalo o miest posunie lavu hranicu pola doprava
	else shiftuj(pole, -mensie, start, (end-mensie)); // inak posunie pravu hranicu pola dolava a naznacuje otacanie pola dolava
	return 0;
}
