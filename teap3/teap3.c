/* Jan Garaj
 * odovodnenie preco je to najlepsie riesenie 
 * teap zadanie cislo 3 - z1535
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "const.h"

void strip(char *buf); 
void head(char *buf);
void tail(char *buf); 
void mergesort(double *pole, int dlzka, double *second);

int main() 
{	
	FILE *f_in;
	char buffer[MAX_LINE_LENGTH], *d_start, *d_end, *endp;
	double *pole_start, *pole_end, zakryte;
	int pocet_dier, line, i, d, pocet_kobercov;

	d = 10; //dlzka koberca

	//otvorenie suboru
	if((f_in=fopen(INPUT_FILE,"r"))==NULL) {
		fprintf(stderr,"Error, opening %s\n",INPUT_FILE);
		exit(1);
	}
	//priprava premennych
	pocet_dier = 0;
	line = 0;
	if((pole_start = (double *) malloc(sizeof(double)))==NULL) {
		fprintf(stderr,"Error, malloc pole_start\n");
		exit (1);
	}
	if((pole_end = (double *) malloc(sizeof(double)))==NULL) {
		fprintf(stderr,"Error, malloc pole_end\n");
		exit (1);
	}
	//parsovanie dat
	while( (fgets(buffer, MAX_LINE_LENGTH, f_in)) != NULL) {
		line++;
		strip(buffer);
		//komentare a prazdna riadky preskocim
		if(buffer[0]=='#' || buffer[0]=='\0') continue;
		d_start = strtok(buffer,"\t");
		d_end = strtok(NULL,"");
		strip(d_start);
		strip(d_end);
		//ak daky problem continue na dalsi riadok
		if(d_start==NULL || d_end==NULL || !strcmp(d_start,"") || !strcmp(d_end,"")) {
			fprintf(stderr,"Error, parsing data in %s at line %d\n",INPUT_FILE, line);
			continue;
		}
		pocet_dier++;
		pole_start = (double *) realloc(pole_start, pocet_dier * sizeof(double));
		pole_end = (double *) realloc(pole_end, pocet_dier * sizeof(double));
		*(pole_start+pocet_dier-1) = strtod(d_start,&endp);
		*(pole_end+pocet_dier-1) = strtod(d_end,&endp);
		if(*(pole_start+pocet_dier-1)>=*(pole_end+pocet_dier-1)) {
			printf("Nespravne zadana poloha diery na riadku %d !!!\n",line);
		}
	}
	//vypis nacitanych dat
	printf("--------------\nPole pred triedenim\n");
	for(i=0;i<pocet_dier;i++) {
		printf("%d. diera %g \t %g\n", i+1, *(pole_start+i), *(pole_end+i));
	}
	//triedenie poli - triedi sa podla pole_start - obdobne sa vsak presuva aj v pole_end
	mergesort(pole_start, pocet_dier, pole_end);
	//vypis utriedenych dat
	printf("--------------\nPole po triedenim\n");
	for(i=0;i<pocet_dier;i++) {
		printf("%d. diera %g \t %g\n", i+1, *(pole_start+i), *(pole_end+i));
	}
	//vlastny algoritmus hladania najmensieho poctu kobercekov 
	zakryte = 0;					//po prvu dieru nemusim nic riesit
	i = 0;										//citac spracovanych dier
	pocet_kobercov = 0;							//citac poctu kobercov
	printf("-------------\nHladanie dier\n");
	while(zakryte<=*(pole_end+pocet_dier-1)) {	//pokial nezakryjem koniec poslednej diery, abo podmienka na pocet dier
		//ak som ciastocne zakryl dieru, nastavim jej zaciatok na koniec zakrytia
		if(*(pole_start+i)<zakryte) *(pole_start+i) = zakryte;			
		if(*(pole_end+i)<zakryte) { 			//zakryl som aj celu nasledujucu dieru predtym, nemam co robit idem next
			i++;
			continue;
		}
		zakryte = *(pole_start+i);				//posuniem sa na zaciatok diery
		do {									//zakryvam, kym nie je zakryta cela diera
			zakryte += 1;
			pocet_kobercov++;
			printf("%d. koberec na %g az %g\n",pocet_kobercov,zakryte-1,zakryte);
		} while(zakryte<=*(pole_end+i));
		i++;
	} //end whille
	printf("=============\nCelkovo je potrebnych %d kobercekov na zakrytie dier.\n",pocet_dier);
	free((void *) pole_start);
	free((void *) pole_end);
	fclose(f_in);

	return 0;
}


void strip(char *buf) 
{
	head( buf );
    tail( buf );
}

void head(char *buf) 
{
    int l;
	
	if( buf == NULL ) return;
	l = strlen( buf );
	while((buf[0] <= 0x20 ) && (l > 0)) {
		int x;
		for( x = 0; x < l; x++ )
			buf[x] = buf[x+1];
		buf[l-1] = '\0';
		l = strlen(buf);
	  	}
}

void tail(char *buf) 
{
	int l;
	int i;

	if(buf == NULL) return;
	l = strlen( buf ) - 1;
	for( i = l; i >= 0; i-- ) {
		if( buf[i] <= 0x20 )
			buf[i] = '\0';
		else
			break;
	}
}

void mergesort(double *pole, int dlzka, double *second) 
{
	double *lava, *prava, *slava, *sprava;
	int i, dlzka_lava, dlzka_prava, i_lava, i_prava;

	if(dlzka > 1) {
		dlzka_lava = dlzka/2;
		dlzka_prava = dlzka - dlzka_lava;
		if ((lava = (double *) malloc(dlzka_lava * sizeof(double))) == NULL) {
			fprintf(stderr,"Malo pamate\n");
			exit(1);
		}
		if ((prava = (double *) malloc(dlzka_prava * sizeof(double))) == NULL) {
			fprintf(stderr,"Malo pamate\n");
			exit(1);
		}
		if ((slava = (double *) malloc(dlzka_lava * sizeof(double))) == NULL) {
			fprintf(stderr,"Malo pamate\n");
			exit(1);
		}
		if ((sprava = (double *) malloc(dlzka_prava * sizeof(double))) == NULL) {
			fprintf(stderr,"Malo pamate\n");
			exit(1);
		}
		/*kopirovanie do podpoli*/
		for(i = 0; i < dlzka_lava; i++) {
			lava[i] = pole[i];
			slava[i] = second[i];
			prava[i] = pole[i + dlzka_lava];
			sprava[i] = second[i + dlzka_lava];
		}
		if(dlzka_lava != dlzka_prava) {
			for(i = dlzka_lava; i < dlzka_prava; i++) {
				prava[i] = pole[i + dlzka_lava];
				sprava[i] = second[i + dlzka_lava];
			}
		}
		/*rekurzivne volanie*/
		mergesort(lava,dlzka_lava, slava);
		mergesort(prava,dlzka_prava, sprava);
		/*zlucenie podpoli*/
		i_lava = 0;
		i_prava = 0;
		for(i = 0; i < dlzka; i++) {
			if(i_lava > dlzka_lava-1) {
				pole[i] = prava[i_prava];
				second[i] = sprava[i_prava];
				i_prava++;
			} 
			else {
				if(i_prava > dlzka_prava-1) {
					pole[i] = lava[i_lava];
					second[i] = slava[i_lava];
					i_lava++;
				}
				else {
					if(lava[i_lava] < prava[i_prava]) { /*kopirovanie spravne usporiadanych poli na spravne miesta*/
						pole[i] = lava[i_lava];
						second[i] = slava[i_lava];
						i_lava++;
					}
					else {
						pole[i] = prava[i_prava];
						second[i] = sprava[i_prava];
						i_prava++;
					}
				}
			}
		}

	}
}
