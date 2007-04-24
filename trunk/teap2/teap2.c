#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>

#define CISLO 10

int i, j, k;
int m = 1;
int kombinacie[256][256];

//zisti mozne kombinacie na zaklade N zo vstupu
int kombinate(int vstup) {
	int pom = vstup;

	for(i=0;i<vstup;i++){
		if(kombinacie[i][m - 1] != 1){
			memcpy(kombinacie[pom], kombinacie[i], m * sizeof(int));
			//printf("l:%d kombinuj kombinacie[%d][%d]=1\n",vstup,pom,m);
			kombinacie[pom][m] = 1;
			pom++;
		}
	}
	m++;
	return pom;
}

//zisti, ci si 2 kombinacie "zavadzaju"
int over(int *kombinacia1, int *kombinacia2, int n) {
	int status = 0;
	int temp[256], i;

	memset(temp, 0, 256 * sizeof(int));
	for(i=0;i<n;i++){
		if(kombinacia1[i] == 1){
			if(i > 0)
			  temp[i - 1] = 1;
			if(i < (n - 1) )
			  temp[i + 1] = 1;
			temp[i] = 1;
		}
	}
	for(i=0;i<n;i++){
		if(temp[i] == 1 && kombinacia2[i] == 1){
			status = 1;
			break;
		}
	}
	return status;
}

int main(void) {
	int debug = 0;				//1 - vypisuje informacie o vyplnani a vypocte table
	int n, l, f, sucet, overenie;
	int x = 2;
	int max = 0;
	int vstup[100][100];
	int table[256][256];
	time_t time_data;

	printf("Zadaj rozmer matice n x n: ");
	scanf("%d", &n);
	//random vstup
	srand(time(&time_data));
	for(l=0;l<n;l++){
		for(j=0;j<n;j++){
			vstup[l][j] = rand() % CISLO;		//obmedzenie ciselneho rozasahu hodnot
		}
	}
	printf("Vstup:\n");
	//vygenerovany vstup
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			printf("%d  ", vstup[i][j]);
		}
		printf("\n");
	}
	//init na kombinacie
	kombinacie[1][0] = 1;
	//vypocet kombinacii
	for(l=1;l<n;l++){			
		x = kombinate(x);
	}
	//vypis mozne kombinacii
	printf("\nVsetky kombinacie:\n");
	for(l=0;l<x;l++){
		for(f=0;f<n;f++){
			printf("%d ", kombinacie[l][f]);
		}
		printf("\n");
	}
	//vypocet 1. riadku
	for(j=0;j<x;j++){
		sucet=0;
		for(i=0;i<n;i++){
			if(kombinacie[j][i] == 1){
				sucet = sucet + vstup[0][i];
			}
		}
		table[0][j] = sucet;
	}
	//hlavny loop
	//vyplnanie matice ciastkovych suctov
	for(i=1;i<n;i++){												//pre kazdy riadok

		for(j=0;j<x;j++){											//pre kazdu kombinaciu1
			for(k=0;k<x;k++){										//pre kazdu kombinaciu2

				overenie = over(kombinacie[j], kombinacie[k], n);	//over ci mozem dane dve kombinacie zobrat do uvahy
				if(!overenie){										//ano mozem zobrat idem do ifu inak dalsia iteracia
					if(debug) printf("%d:table[%d][%d]:\n",(i*x)+j,i,j);
					if(debug) printf("mozem zobrat kombinacie[%d] a [%d]\n",j,k);
					sucet = table[i - 1][k];						//hodnota z predchazajuceho riadku
					if(debug) printf(" sucet je %d z table[%d][%d]\n",table[i-1][k],i-1,k);
					for(l=0;l<n;l++){								//prejdem celu konkretnu kombinaciu 
						if(kombinacie[j][l] == 1) {					//ak ma dana kombinacia "zobrate" cislo
						  sucet = sucet + vstup[i][l];				//pripocitaj prislusny vstup k doterajsiemu suctu
						  if(debug) printf("  novy sucet je %d = old sucet + vstup[%d][%d]...%d\n",sucet, i, l, vstup[i][l]);
						}
					}
					if(debug) printf("-test ci sucet %d > table[%i][%d] %d\n",sucet,i,j,table[i][j]);
					if(sucet > table[i][j]) {						//ak vypocitany sucet je vacsi ako table
						if(debug) printf(" -test pozitivny table[%d][%d] je rovna %d\n", i,j,sucet);
						table[i][j] = sucet;						//dam do matice sucet - maximum z sucet a mtiac
					}
				}

			}

		}
	}
	//printf("\nkombinacii je %d\n", x);
	printf("\nVystup:\n");
	//prejdem table a najdem jej maximum
	for(i=0;i<n;i++){
		for(j=0;j<x;j++){
			if(i == (n - 1) ){
				if(table[i][j] > max)
				  max = table[i][j];
			}
			printf("%3d", table[i][j]);
		}
		printf("\n");
	}
	printf("\nMaximalny mozny objem je: %d\n", max);
	return 0;
}
