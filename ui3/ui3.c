/* Jan Garaj
   Zadanie cislo 3 z UI
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <string.h>
#include <assert.h>

#include "const.h"
#include "genotyp.h"
#include "/home/vlk/bin/include/matheval.h"

int main(void) 
{
	genotyp *generacia, *jedinec, *pomocny_jedinec;
	gen *pomocny_gen;
	char *pole_genov[POCET_GENOV], naj_funkcia[MAX_LENGHT_FUNCTION], in_funkcia[MAX_LENGHT_FUNCTION];
	int i, tikov_za_sec, gen, alg;
	double pom_fitnes, priem_fitnes, naj_fitnes, sum, sum_ruleta;
	struct tms casova_struktura;
	clock_t celkovy_z, celkovy_k, program_z, program_k;
	FILE *f_priem, *f_naj;

	f_priem = fopen("priem_fitnes.txt","w");
	f_naj = fopen("naj_fitnes.txt","w");

	naj_fitnes = HUGE_VAL;		//infinite
	printf("Zadaj funkciu, ktoru budem aproximovat:");
	gets(in_funkcia);
	double hodnoty_y[17];
	void *f;
	f = evaluator_create(in_funkcia);
	assert (f);
	for(i=0;i<16;i++) {
		hodnoty_y[i] = evaluator_evaluate_x(f,i);
	}
	printf("Vyber pouzity sposob selekcie (1-turnaj, 2-orezanim):");
	scanf("%d",&alg);
	//inicializacia pole_genov
	for(i=0;i<POCET_GENOV;i++) {
		pole_genov[i]=(char *) malloc(3*sizeof(char));
	}
	*(pole_genov+0) = G1;
	*(pole_genov+1) = G2;
	*(pole_genov+2) = G3;
	*(pole_genov+3) = G4;
	*(pole_genov+4) = G5;
	*(pole_genov+5) = G6;
	*(pole_genov+6) = G7;
	*(pole_genov+7) = G8;
	*(pole_genov+8) = G9;
	*(pole_genov+9) = G10;
	*(pole_genov+10) = G11;
	//init casovych udajov
	tikov_za_sec=sysconf(_SC_CLK_TCK);
	celkovy_z = times(&casova_struktura);
	program_z = casova_struktura.tms_utime;
	//init generacia 0
	if((generacia=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
		fprintf(stderr,"Error, malloc generacia\n");
		exit(1);
	}
	pomocny_jedinec=generacia;
	pomocny_jedinec->geny = random_geny(pole_genov, pomocny_jedinec);
	for(i=1;i<POCET_JEDINCOV;i++) {
		if((jedinec=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
			fprintf(stderr,"Error, malloc jedinec\n");
			exit(1);
		}
		jedinec->geny = random_geny(pole_genov,jedinec);
		pomocny_jedinec->next=jedinec;
		pomocny_jedinec=jedinec;
	}
	pomocny_jedinec->next=NULL;
	//vypis genotypov a genov generacie novej generaci
	pomocny_jedinec = generacia;
	priem_fitnes = 0;
	sum_ruleta = 0;
	for(i=0;i<POCET_JEDINCOV;i++) {
		pomocny_gen = pomocny_jedinec->geny;
		pomocny_jedinec->funkcia = urob_funkciu(pomocny_jedinec,0);
		pomocny_jedinec->fitnes = 0;
		int l;
		sum = 0;
		pom_fitnes = 0;
		for(l=0;l<POCET_X;l++) {
			pom_fitnes = evalvuj(pomocny_jedinec->funkcia, l+1);
			if(isinf(pom_fitnes) || isnan(pom_fitnes)) {
				pomocny_jedinec->fitnes = HUGE_VAL;
				break;
			}
			sum += ((pom_fitnes-hodnoty_y[l])*(pom_fitnes-hodnoty_y[l]));
		}
		if(!isinf(pomocny_jedinec->fitnes)) {
			pomocny_jedinec->fitnes = sum/POCET_X;
		}
		if(isnan(pomocny_jedinec->fitnes)) pomocny_jedinec->fitnes = HUGE_VAL;
		if(!isinf(pomocny_jedinec->fitnes)) {
			priem_fitnes += pomocny_jedinec->fitnes;
		}
		if(naj_fitnes>pomocny_jedinec->fitnes){
			naj_fitnes = pomocny_jedinec->fitnes;
			strcpy(naj_funkcia, pomocny_jedinec->funkcia);
		}
		printf("\n---------------------------\ngenotyp no:%d ma geny:\n",i);
 		while(pomocny_gen->next!=NULL) {
			printf("%s",pomocny_gen->genik);
			pomocny_gen = pomocny_gen->next;
		}
		//jednogenovy genotyp
		if(pomocny_gen->next==NULL && pomocny_gen->genik!=NULL) {
			printf("%s",pomocny_gen->genik);
		}
		printf("\nVytvorena fcia pre genotyp %s\n", pomocny_jedinec->funkcia);
		printf("Fitnes %g\n",pomocny_jedinec->fitnes);
		printf("dlzka genikov %d\n",pomocny_jedinec->dlzka);
		pomocny_jedinec=pomocny_jedinec->next;
	}
	priem_fitnes = priem_fitnes/POCET_X;
//printf("priem fitnes: %g\n",priem_fitnes);
//printf("Naj fitnes generacie je %g\n", naj_fitnes);
	//hlavny evolucny cyklus
	gen = 0;
	while(gen<MAX_GENERACII) {
printf("++++generacia %d ++++++++++++++++++++++++++++++++++++++++++++++++++\n",gen);
		gen++;
		if(naj_fitnes==0) break;	//test na najlepsi fitnes, ak 0 nemam co riesit mam "asi" hladanu fciu
/******** BODY ****************/
		if(alg==1) {
			generacia = turnaj(generacia, pole_genov);
		} 
		else {
		//generacia = ruleta(generacia, sum_ruleta);
			generacia = zotried(generacia); 
			generacia = orezanie(generacia, pole_genov);
		}
/********* END BODY *************/
/********** VYPIS ************************************************************************************************/
		pomocny_jedinec = generacia;
		priem_fitnes = 0;
		sum_ruleta = 0;
		for(i=0;i<POCET_JEDINCOV;i++) {
			pomocny_gen = pomocny_jedinec->geny;
			pomocny_jedinec->funkcia = urob_funkciu(pomocny_jedinec,1);
			pomocny_jedinec->fitnes = 0;
			int l;
			double sum;
			sum = 0;
			pom_fitnes = 0;
			for(l=0;l<POCET_X;l++) {
				pom_fitnes = evalvuj(pomocny_jedinec->funkcia, l+1);
				if(isinf(pom_fitnes) || isnan(pom_fitnes)) {
					pomocny_jedinec->fitnes = HUGE_VAL;
					break;
				}
				sum += ((pom_fitnes-hodnoty_y[l])*(pom_fitnes-hodnoty_y[l]));
			}
			if(!isinf(pomocny_jedinec->fitnes)) {
				pomocny_jedinec->fitnes = sum/POCET_X;
			}
			if(isnan(pomocny_jedinec->fitnes)) pomocny_jedinec->fitnes = HUGE_VAL;
			if(!isinf(pomocny_jedinec->fitnes)) {
				priem_fitnes += pomocny_jedinec->fitnes;
			}
			if(naj_fitnes>pomocny_jedinec->fitnes){
				naj_fitnes = pomocny_jedinec->fitnes;
				strcpy(naj_funkcia, pomocny_jedinec->funkcia);
			}
			printf("---------------------------\nTgenotyp no:%d ma geny:\n",i);
 			while(pomocny_gen->next!=NULL) {
				printf("%s",pomocny_gen->genik);
				pomocny_gen = pomocny_gen->next;
			}
			//jednogenovy genotyp
			if(pomocny_gen->next==NULL && pomocny_gen->genik!=NULL) {
				printf("%s",pomocny_gen->genik);
			}
printf("\nVytvorena fcia pre genotyp %s\n", pomocny_jedinec->funkcia);
printf("Fitnes %g\n",pomocny_jedinec->fitnes);
printf("Dlzka geny je %d\n", pomocny_jedinec->dlzka);
			pomocny_jedinec=pomocny_jedinec->next;
		}
		priem_fitnes = priem_fitnes/POCET_X;
		printf("priem fitnes: %g\n",priem_fitnes);
		fprintf(f_priem,"%g\n",priem_fitnes);
		fprintf(f_naj,"%g\n",naj_fitnes);
	}  //END FOR PRE MAX GENERACIE
/************* END VYPIS **********************/
	celkovy_k = times(&casova_struktura);
	program_k = casova_struktura.tms_utime;
	//vypis casomiery
	printf("==================================\n");
	printf("Najlepsi dosiahnuty fitnes %g\n",naj_fitnes);
	printf("Najlepsia najdena funkcia %s\n",naj_funkcia);
	printf("Celkovy cas = %.3f [sec]\n",(celkovy_k - celkovy_z) / (double) tikov_za_sec);
	printf("Procesorovy cas = %.3f [sec]\n",(program_k - program_z) / (double) tikov_za_sec);
	free((void *) generacia);
	fclose(f_priem);
	fclose(f_naj);


	return 0;
}
