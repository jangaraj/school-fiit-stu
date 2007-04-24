#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <math.h>

#include "const.h"
#include "genotyp.h"
#include "/home/vlk/bin/include/matheval.h"

gen *random_geny(char *pole_genov[], genotyp *jedinec)
{
	gen *output, *j_gen, *pomocny_gen;
	int i, n, random;
	struct timeval a;

	if((output=(gen *) malloc(sizeof(gen)))==NULL) {
		fprintf(stderr,"Error, malloc gen\n");
		return NULL;
	}
	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	random = rand() % MAX_GENOV;
//printf("Dlzka genu bude %d\n", random+1);	
	if(random<2) random = 2;
	jedinec->dlzka = random+1;
	pomocny_gen = output;
	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	n = rand() % POCET_GENOV;
	pomocny_gen->genik = pole_genov[n];
	pomocny_gen->prev = NULL;
	pomocny_gen->next = NULL;
//printf("z pola genov vyberam genik %s\n", pole_genov[n]);
	for(i=0;i<random;i++) {				//cyklus na random pocet genov
		if((j_gen=(gen *) malloc(sizeof(gen)))==NULL) {
			fprintf(stderr,"Error, malloc pomocny_gen\n");
			return NULL;
		}
		pomocny_gen->next = j_gen;
		n = rand() % POCET_GENOV;
//printf("z pola genov vyberam genik %s\n", pole_genov[n]);
		j_gen->genik = pole_genov[n];
		j_gen->prev = pomocny_gen;
		pomocny_gen=j_gen;
	}
	if(random > 0) j_gen->next=NULL;
	return output;
}

char *urob_funkciu(genotyp *jedinec, int recheck) 
{	
	char *output;
	genotyp *p_jedinec,	point_jedinec;
	gen *j_gen, *p_gen;
	int flag, i, length;
	//char *p_string;

	p_jedinec = jedinec;
	p_gen = jedinec->geny;
	point_jedinec = *jedinec;
	if((output=(char *) malloc(MAX_LENGHT_FUNCTION*sizeof(char)))==NULL) {
		fprintf(stderr,"Error, malloc output of urob_funkciu\n");
		return NULL;
	}
	//preprasujem geny na string 
	p_jedinec = &point_jedinec;
	flag = 0; 	//na kontrolu zatvoriek
//printf("flag pred parse %d\n",flag);
	length = 0; //pocet genov od otvorenia zatvorky , aby som neurobil ()
	//hlavny parsovaci while
//	while(p_jedinec->geny->next != NULL) {
	for(i=0; i<p_jedinec->dlzka && p_jedinec->geny->next != NULL; i++) {
		if(strcmp(G1,p_jedinec->geny->genik)==0) {
			output = strcat(output,"+1");
			length++;
		}
		if(!strcmp(G2,p_jedinec->geny->genik)) {
			output = strcat(output,"-3");
			length++;
		}
		if(!strcmp(G3,p_jedinec->geny->genik)) {
			output = strcat(output,"*2");
			length++;
		}
		if(!strcmp(G4,p_jedinec->geny->genik)) {
			output = strcat(output,"/3");
			length++;
		}
		if(!strcmp(G5,p_jedinec->geny->genik)) {
			output = strcat(output,"+x");
			length++;
		}
		if(!strcmp(G6,p_jedinec->geny->genik)) {
			output = strcat(output,"-x");
			length++;
		}
		if(!strcmp(G7,p_jedinec->geny->genik)) {
			output = strcat(output,"*x");
			length++;
		}
		if(!strcmp(G8,p_jedinec->geny->genik)) {
			output = strcat(output,"/x");
			length++;
		}
		if(!strcmp(G9,p_jedinec->geny->genik) && (length>0)) {	
			output = strcat(output,"*(");
			flag++;
//printf("flag++ %d *(\n",flag);
			length = 0;
		}
		if(!strcmp(G10,p_jedinec->geny->genik) && (length>0)) {
			output = strcat(output,"/(");
			flag++;
//printf("flag++ %d /(\n",flag);
			length = 0;
		}
		if(!strcmp(G11,p_jedinec->geny->genik) && flag>0 && length>0 && recheck==0) {
			output = strcat(output,")");
			flag--;
//printf("flag-- %d )\n",flag);
			length++;
		}
		/*if(!strcmp(G11,p_jedinec->geny->genik) && flag>0) {
			flag--;
printf("flag-- %d BEZ PRIDANIA DO FCIE\n",flag);
		}*/
		p_jedinec->geny = p_jedinec->geny->next;
	} 
//printf("preparse flag: %d\n", flag);
	//aby som nemal na konci prazdne zatvorene zatvorky dam aspon +3-x
	if(length==0 && flag>0) {
		output = strcat(output,"+3-x");
		//flag--;
	}
	//dozatvaranie zatvoriek
	if(flag>0) {
		for(i=0;i<flag;i++) {				
//printf("zatvaricfor flag %d\n",flag);
			//vratim sa o flag spat a pozrem ci tam nie je zatvaracia zatvorka
			int seek=1;
			while(seek<flag && p_jedinec->geny->prev!=NULL) {
				p_jedinec->geny = p_jedinec->geny->prev;
				seek++;
			}
			if(!strcmp(G11,p_jedinec->geny->genik)){
				//flag--;
				seek=1;
				while(seek<flag) {
					p_jedinec->geny = p_jedinec->geny->next;
					seek++;
				}
				continue;
			}
			//seek na povodnu hodnotu
			seek=1;
			while(seek<flag) {
				seek++;
				p_jedinec->geny = p_jedinec->geny->next;
			}

			if((j_gen=(gen *) malloc(sizeof(gen)))==NULL) {
				fprintf(stderr,"Error, malloc pomocny_gen\n");
				return NULL;
			}
			p_jedinec->geny->next = j_gen;
			j_gen->genik = G11; //)
			j_gen->prev = p_jedinec->geny;
			p_jedinec->geny = j_gen;
			output = strcat(output,")");
//printf("Zavieram odl dlzka %d ",jedinec->dlzka);
			jedinec->dlzka++;
//printf(" new dlzka %d\n",jedinec->dlzka);
//printf("zavieram )\n");			
		}
		p_jedinec->geny->next = NULL;		
	}
	//ak som neurobil nic, dam tam aspon G1 aby libmatheval nefrflal
	if(!strcmp(output,"")) output = strcat(output,"1");
	//odtsranenie / * + - zo zaciatku
	flag=0;   // pozicia pokial je na zaciatku / alebo *
    for(i=0; i<strlen(output); i++) {
	    if(!strncmp((output+i),"*",1) || !strncmp((output+i),"/",1) || !strncmp((output+i),"+",1) || !strncmp((output+i),"-",1)) {
			flag++;	
	    }
		break;
	}
    for(i=0; i<flag; i++) output++;
	//odstranenie / * + - z pred otvaracimi zatvorkami, bo to strasne vadi libmatheval :)
    for(i=0; i<strlen(output); i++) {
	    if((!strncmp((output+i),"*",1) && (!strncmp((output+i-1),"(",1))) || (!strncmp((output+i),"/",1) && (!strncmp((output+i-1),"(",1))) || (!strncmp((output+i),"+",1) && (!strncmp((output+i-1),"(",1))) || (!strncmp((output+i),"-",1) && (!strncmp((output+i-1),"(",1)))) {
			*(output+i)=' ';
			//TODO odstranit medzeru ay to krajsie vyzeralo :)
			//strncpy(p_string,output,i-1);
			//output=strcat((output+i),p_string);
	    }
	}
	//zaverecne kontrola na zatvaraciu zatvorku
	flag = 0;
 	for(i=0; i<strlen(output); i++) {
		if(!strncmp((output+i),"(",1)) flag++;		
		if(!strncmp((output+i),")",1)) flag--;
	}
//printf("2output pre zatvorky 2 %s\n",output);
	//dozatvaranie zatvoriek2
	if(flag>0) {
		for(i=0;i<flag;i++) {				
//printf("2zatvaricfor ionly vo fcii flag %d\n",flag);
			output = strcat(output,")");
//printf("2Zavieram odl dlzka %d ",jedinec->dlzka);
//printf("2 new dlzka %d\n",jedinec->dlzka);
//printf("2zavieram )\n");			
		}
	}
	return output;
}

double evalvuj(char *fcia, int x)
{
	void *f;
//printf("Evalfcia %s\n",fcia);
	f = evaluator_create(fcia);
	assert (f);
	return evaluator_evaluate_x(f,x);
}

genotyp *turnaj(genotyp *in, char *pole_genov[])
{	
	genotyp *generacia, *pomocny_jedinec, *jedinec, *out, *g1, *g2;
	struct timeval a;
	int rand1, rand2, i;

	if((generacia=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
		fprintf(stderr,"Error, malloc generacia\n");
		exit(1);
	}
	pomocny_jedinec=generacia;
	rand2 = 0;
	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	rand1 = rand() % POCET_JEDINCOV;
	do {
		gettimeofday(&a,NULL);
		srand(a.tv_usec);
		rand2 = rand() % POCET_JEDINCOV;
	} while(rand1==rand2);  //UZ JE OK
	g1 = g2 = in;
	while(g1->next != NULL && rand1>0) {
		rand1--;
		g1 = g1->next;
	}
	while(g2->next != NULL && rand2>0) {
		rand2--;
		g2 = g2->next;
	}
	if(g1->fitnes>=g2->fitnes) {
		//vyberem g2
		out = g2;
		pomocny_jedinec->geny = g2->geny;
		pomocny_jedinec->funkcia = g2->funkcia;
		pomocny_jedinec->fitnes = g2->fitnes;
		pomocny_jedinec->dlzka = g2->dlzka;
	}
	else {
		//vyberem g1
		out = g1;
		pomocny_jedinec->geny = g1->geny;
		pomocny_jedinec->funkcia = g1->funkcia;
		pomocny_jedinec->fitnes = g1->fitnes;
		pomocny_jedinec->dlzka = g1->dlzka;
	}
	i = 1;
	while(i<POCET_JEDINCOV) {
		if((jedinec=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
			fprintf(stderr,"Error, malloc jedinec\n");
			exit(1);
		}
		rand2 = 0;
		gettimeofday(&a,NULL);
		srand(a.tv_usec);
		rand1 = rand() % POCET_JEDINCOV;
		do {
			gettimeofday(&a,NULL);
			srand(a.tv_usec);
			rand2 = rand() % POCET_JEDINCOV;
		} while(rand1==rand2); //UZ JE OK
//printf("rand1 %d\nrand2 %d\n",rand1,rand2);
		g1 = g2 = in;
		while(g1->next != NULL && rand1>0) {
			rand1--;
			g1 = g1->next;
		}	
		while(g2->next != NULL && rand2>0) {
			rand2--;
			g2 = g2->next;
		}
		if(g1->fitnes>=g2->fitnes) {//vyberem g2
			i++;
			jedinec->geny = g2->geny;
			jedinec->funkcia = g2->funkcia;
			jedinec->fitnes = g2->fitnes;
			jedinec->dlzka = g2->dlzka;
		}
		else {						//vyberem g1
			i++;
			jedinec->geny = g1->geny;
			jedinec->funkcia = g1->funkcia;
			jedinec->fitnes = g1->fitnes;
			jedinec->dlzka = g1->dlzka;
		}
		pomocny_jedinec->next=jedinec;
		pomocny_jedinec=jedinec;
		//nahodna mutacia
		gettimeofday(&a,NULL);
		srand(a.tv_usec);
		rand1 = rand() % 100;			//mutovat ci nemutovat?
		rand1++;
		if(rand1<=PRAVDEPODOBNOST_MUTACIE){
//printf("XXdlzka pred mutaciou %d\n", jedinec->dlzka);			
			jedinec->geny = mutacia(jedinec,pole_genov);
//printf("XXdlzka po    mutacii %d\n",jedinec->dlzka);
			rand1 = rand1;
		}
//printf("g1fitnes %g\ng2fitnes %g\n",g1->fitnes,g2->fitnes);
	}
	pomocny_jedinec->next=NULL;
	pomocny_jedinec = generacia;
	free((void *) in);
	return generacia;
}

gen *mutacia(genotyp *in, char *pole_genov[])
{
	struct timeval a;
	int rand1, rand2;
	gen *mut_gen, *new_gen;

	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	rand1 = rand() % 3;			//vyber druhu mutacie
	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	rand2 = rand() % in->dlzka;	//index mutacie
	mut_gen = in->geny;
	//seek
	while(mut_gen->next != NULL && rand2>0) {
		rand2--;
		mut_gen = mut_gen->next;
	}
	switch(rand1) {
		case 0:					//vymena genu za iny nahodny
			gettimeofday(&a,NULL);
			srand(a.tv_usec);
			rand2 = rand() % (POCET_GENOV-1);	//index noveho genu
			mut_gen->genik = pole_genov[rand2];
//printf("mutujem vymenou genu dam novy gen %s\n",pole_genov[rand2]);
			break;
		case 1:					//odstranenie nahodneho genu
			if(mut_gen->prev!=NULL && mut_gen->next!=NULL) {
				if(mut_gen->prev==NULL && mut_gen->next!=NULL) {
					mut_gen->next->prev = NULL;
					free((void *) mut_gen);
					in->dlzka--;
//printf("mutujem odstranenim genu\n");
					break;
				}
				if(mut_gen->next==NULL && mut_gen->prev!=NULL)  {
					mut_gen->prev->next = NULL;
					free((void *) mut_gen);
					in->dlzka--;
//printf("mutujem odstranenim genu\n");
					break;
				}
				if(mut_gen->next!=NULL && mut_gen->prev!=NULL) {
					mut_gen->next->prev = mut_gen->prev;
					mut_gen->prev->next = mut_gen->next;
					free((void *) mut_gen);
					in->dlzka--;
//printf("mutujem odstranenim genu\n");
					break;
				}
			}
			break;
		case 2:					//pridanie nahodneho genu
			if(in->dlzka>MAXIMUM_GENOV_V_GENOTYPE) break;
			if((new_gen=(gen *) malloc(sizeof(gen)))==NULL) {
				fprintf(stderr,"Error, malloc new mutant gen\n");
				exit (1);
			}
			gettimeofday(&a,NULL);
			srand(a.tv_usec);
			rand2 = rand() % (POCET_GENOV-1);	//index noveho genu
			new_gen->genik = pole_genov[rand2];
			if(mut_gen->next==NULL) {
				new_gen->next = NULL;
				new_gen->prev = mut_gen;
				mut_gen->next=new_gen;
				in->dlzka++;
//printf("mutujem pridanim genu na dlzku %d\n",in->dlzka);
				break;
			}
			mut_gen->next->prev = new_gen;
			new_gen->next = mut_gen->next;
			new_gen->prev = mut_gen;
			mut_gen->next=new_gen;
			in->dlzka++;
//printf("mutujem pridanim genu na dlzku %d\n",in->dlzka);
			break;
		default:
			fprintf(stderr,"Error pri switchovani v mutacii\n");
			break;
	}
	return in->geny;
}

genotyp *zotried(genotyp *in)
{
	genotyp *max, *min, *del, *pom, *pomocny_jedinec, *jedinec;
	int i;

	if((jedinec=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
		fprintf(stderr,"Error, malloc generacia\n");
		exit(1);
	}
//	pomocny_jedinec = ;
	jedinec->geny = in->geny;
	jedinec->funkcia = in->funkcia;
	jedinec->fitnes = in->fitnes;
	jedinec->dlzka = in->dlzka;
	min = max = jedinec; 
	pomocny_jedinec = in->next;
//printf("nove max min je %g\n",max->fitnes);
	i = 1;
	while(i<POCET_JEDINCOV) {
		i++;
		if((jedinec=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
			fprintf(stderr,"Error, malloc jedinec\n");
			exit(1);
		}
		jedinec->geny = pomocny_jedinec->geny;
		jedinec->funkcia = pomocny_jedinec->funkcia;
		jedinec->fitnes = pomocny_jedinec->fitnes;
		jedinec->dlzka = pomocny_jedinec->dlzka;
//	p_jedinec = in->next;
//	for(i=0;i<(POCET_JEDINCOV-1);i++) {
//printf("Idem spracovat fitnes %g\n",jedinec->fitnes);
		if(jedinec->fitnes >= max->fitnes) {		//mam najnovise maximum
			max->next = pomocny_jedinec;
			max = pomocny_jedinec;
//printf("Nove max je %g\n",max->fitnes);
		} else {
			if(jedinec->fitnes <= min->fitnes) {	//mam naknovsie minimum
				jedinec->next = min;
				min = jedinec;
//printf("Nove min je %g\n",min->fitnes);
			} else {								//seekujem kym nie som na spravnom mieste	
				pom = min;
				while(pom->fitnes < jedinec->fitnes && pom->next->fitnes <= jedinec->fitnes) {
					pom = pom->next;
				}
				jedinec->next = pom->next;
				pom->next = jedinec;
//printf("pcham ho do stredu\n");
			}
		}
		del = pomocny_jedinec;
		pomocny_jedinec = pomocny_jedinec->next;
		//free((void *) del);
	}
	max->next = NULL;
	return min;
}

genotyp *ruleta(genotyp *in, double sum)
{
	double treshold, local_sum;
	struct timeval a;
	int i;
	genotyp *jedinec, *pomocny_jedinec, *generacia, *p_jedinec;

	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	treshold = rand() % (int)sum; //treshold podla ktoreho budem vyberat
	if((generacia=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
		fprintf(stderr,"Error, malloc generacia\n");
		exit(1);
	}
	pomocny_jedinec = generacia;
	while(i<POCET_JEDINCOV) {
		i++;
		p_jedinec = in;
		while(local_sum<treshold) {
			//seeking
			if(p_jedinec->next == NULL) p_jedinec = in;
			local_sum += 1/p_jedinec->fitnes;
			p_jedinec = p_jedinec->next;
		}
		if((jedinec=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
			fprintf(stderr,"Error, malloc jedinec\n");
			exit(1);
		}
		jedinec->geny = p_jedinec->geny;
		jedinec->funkcia = p_jedinec->funkcia;
		jedinec->fitnes = p_jedinec->fitnes;
		jedinec->dlzka = p_jedinec->dlzka;
		pomocny_jedinec->next = jedinec;
		pomocny_jedinec = jedinec;
	}


	free ((void *) in);
	return generacia;
}

//vstupom uz musi byt zotriedena generacia
genotyp *orezanie(genotyp *in, char *pole_genov[])
{	
	genotyp *jedinec, *generacia, *pomocny_jedinec, *p_jedinec, *del;
	int i, selekt, rand1;
	struct timeval a;

	selekt = (int) round(T*POCET_JEDINCOV);
	if((generacia=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
		fprintf(stderr,"Error, malloc generacia - orezanie\n");
		exit(1);
	}
	pomocny_jedinec = generacia;
	pomocny_jedinec->geny = in->geny;
	pomocny_jedinec->funkcia = in->funkcia;
	pomocny_jedinec->fitnes = in->fitnes;
	pomocny_jedinec->dlzka = in->dlzka;
	p_jedinec = in->next;
	for(i=0;i<=selekt;i++) {
		//vybrat aktualnych jedincov do novej generacie
		if((jedinec=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
			fprintf(stderr,"Error, malloc jedinec - orezanie\n");
			exit(1);
		}
		pomocny_jedinec->next = jedinec;
		jedinec->geny = p_jedinec->geny;
		jedinec->funkcia = p_jedinec->funkcia;
		jedinec->fitnes = p_jedinec->fitnes;
		jedinec->dlzka = p_jedinec->dlzka;
		pomocny_jedinec = jedinec;
		p_jedinec = p_jedinec->next;
	}
	//doplnit generaciu krizenim z existujucich
	//zaroven uvolnim zvysok generacie
	for(;i<POCET_JEDINCOV;i++) {
		pomocny_jedinec->next = krizenie(generacia, selekt);
		//mutacia
		gettimeofday(&a,NULL);
		srand(a.tv_usec);
		rand1 = rand() % 100;			//mutovat ci nemutovat?
		rand1++;
		if(rand1<=PRAVDEPODOBNOST_MUTACIE){
			pomocny_jedinec->geny = mutacia(pomocny_jedinec,pole_genov);
		}
		//end mutacia
		pomocny_jedinec = pomocny_jedinec->next;
		del = p_jedinec;
		if(p_jedinec!=NULL) {
			p_jedinec = p_jedinec->next;
			free((void *) del);
		}
	}
	pomocny_jedinec->next = NULL;
	return generacia;
}

genotyp *krizenie(genotyp *in, int pocet) 
{	
	int rand1, rand2, dlzka, i, p1, p2;
	struct timeval a;
	gen *pom, *gg1, *gg2, *out, *gg;
	char *new_genik;
	genotyp *decko, *g1, *g2;

	dlzka = 0;
	if((decko=(genotyp *) malloc(sizeof(genotyp)))==NULL) {
		fprintf(stderr,"Error, malloc decko\n");
		exit(1);
	}
	if((out=(gen *) malloc(sizeof(gen)))==NULL) {
		fprintf(stderr, "Error, malloc gen\n");
		exit(1);
	}
	decko->geny = out;
	pom = out;
	out->prev = NULL;
	rand2 = 0;
	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	rand1 = rand() % pocet;
	do {
		gettimeofday(&a,NULL);
		srand(a.tv_usec);
		rand2 = rand() % pocet;
	} while(rand1==rand2);  //UZ JE OK
	//urcenie rodicov
	g1 = g2 = in;
	while(g1->next != NULL && rand1>0) {
		rand1--;
		g1 = g1->next;
	}
	while(g2->next != NULL && rand2>0) {
		rand2--;
		g2 = g2->next;
	}
	//urcenie pozicii kde sa bude krizit
	p2 = 0;
	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	if(g1->dlzka==0) g1->dlzka++;
	p1 = rand() % g1->dlzka;
	gettimeofday(&a,NULL);
	srand(a.tv_usec);
	if(g2->dlzka==0) g2->dlzka++;
	p2 = rand() % g2->dlzka;
	//krizenie
	//seek geny
	gg1 = g1->geny;
	gg2 = g2->geny;
	if((new_genik=(char *) malloc(3*sizeof(char)))==NULL) {
		fprintf(stderr,"Error, malloc new_genik\n");
		exit(1);
	}
	pom->genik = new_genik;
	strcpy(pom->genik, gg1->genik);
	dlzka++;
	//kopirovanie z prveho genu prvej casti
	while(gg1->next != NULL && p1>0) {
		if((gg=(gen *) malloc(sizeof(gen)))==NULL) {
			fprintf(stderr,"Error, malloc gg\n");
			exit(1);
		}
		pom->next = gg;
		gg->prev = pom;
		if((new_genik=(char *) malloc(3*sizeof(char)))==NULL) {
			fprintf(stderr,"Error, malloc new_genik\n");
			exit(1);
		}
		gg->genik = new_genik;
		strcpy(gg->genik, gg1->genik);
//printf("new first genik %s\n",gg->genik);
		p1--;
		gg1 = gg1->next;
		dlzka++;
		pom = gg;
	}
	//dokopirovanie genov z druheho genu z druhej casti
	//seek v druhom genotype na spravny genik
	for(i=0;i<p2 && gg2->next!=NULL; i++) {
		gg2 = gg2->next;
	}
	while(gg2->next != NULL && dlzka<MAXIMUM_GENOV_V_GENOTYPE) {
		if((gg=(gen *) malloc(sizeof(gen)))==NULL) {
			fprintf(stderr,"Error, malloc gg\n");
			exit(1);
		}
		pom->next = gg;
		gg->prev = pom;
		if((new_genik=(char *) malloc(3*sizeof(char)))==NULL) {
			fprintf(stderr,"Error, malloc new_genik\n");
			exit(1);
		}
		gg->genik = new_genik;
		strcpy(gg->genik, gg2->genik);
//printf("New genik second %s\n",gg->genik);
		gg2 = gg2->next;
		dlzka++;
		pom = gg;
	}
	pom->next = NULL;
	decko->dlzka = dlzka;

	return decko;
}
