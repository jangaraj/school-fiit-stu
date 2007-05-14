#include <stdio.h>
#include <string.h>
#include "struktury.h"
#define MAXLENGTH 80

def_pravidlo *kopiruj(def_pravidlo *pom);
def_pravidlo *insert_pravidlo(def_pravidlo *pom,def_pravidlo *zas_pravidiel);
fakty *remove_fakt(fakty *mnozina,fakty *zas_faktov);
fakty *aplikuj(fakty *mnozina,fakty *zas_pravidiel);
fakty *expanduj(fakty *zas_faktov,def_pravidlo *zas_pravidiel,fakty *mnozina,int x);
fakty *insert_akcie(fakty *mnozina_akcii,fakty *mnozina);
fakty *vloz_fakt(fakty *pom,fakty *zas_faktov);
fakty *odstran(fakty *mnozina,fakty *token);
fakty *vloz_mnozina(char *text,fakty *mnozina);
fakty *vymaz(fakty *pom,fakty *zas_faktov);
int exchange(def_pravidlo *zas_vymen,def_pravidlo *pomocne,fakty *ftoken,int x,int y);
void vypis(fakty *zas_faktov);
bool is_empty(fakty *mnozina);


int main(void)
{
	char veta[MAXLENGTH];
	char znak=0;
	int i=0;
	int zatv=0;
	FILE *infile;
	fakty *zas_faktov;
	fakty *pom_fakt;
	fakty *mnozina,*mnozina_akcii;
	def_pravidlo *zas_pravidiel;
	def_pravidlo *pom_pravidlo;
	def_pravidlo *token;
	zas_faktov=new fakty;
	zas_faktov->fakt[0]=0;
	zas_faktov->next=NULL;
	zas_pravidiel=new def_pravidlo;
	zas_pravidiel->next=NULL;
	zas_pravidiel->meno[0]=0;
	zas_pravidiel->podmienka[0]=0;
	zas_pravidiel->akcia[0]=0;
	
	//NACITANIE FAKTOV
	if((infile=fopen("fakty.txt","rd")) == NULL ) {
		fprintf(stderr,"Error, opening fakty.txt\n");
		return 1;
	}
	else {
		while(fscanf(infile,"%c",&znak)!=EOF) {
			i=0;
			if(znak=='(') {
				zatv++;
				while(zatv!=0) {
					veta[i]=znak;
					fscanf(infile,"%c",&znak);
					if(znak=='(')zatv++;
					if(znak==')')zatv--;
					i++;
				}
				veta[i]=znak;
				veta[i+1]=0;
				pom_fakt=new fakty;
				for(i=0;i<40;i++) {
					pom_fakt->fakt[i]=veta[i];
				}
				pom_fakt->next=NULL;
				zas_faktov=vloz_fakt(pom_fakt,zas_faktov);
			}
		}
		fclose(infile);
	}
	//NACITANIE PRAVIDIEL
	zatv=0;
	if((infile=fopen("pravidla.txt","rd")) == NULL) {
		fprintf(stderr,"Error, opening pravidla.txt\n");
		return 1;
	}
	else {
		while(fscanf(infile,"%c",&znak)!=EOF) {
			i=0;
			pom_pravidlo=new def_pravidlo;
			while(znak!=':') {
				pom_pravidlo->meno[i]=znak;
				i++;
				fscanf(infile,"%c",&znak);
			}
			pom_pravidlo->meno[i]=znak;
			pom_pravidlo->meno[i+1]=0;
			pom_pravidlo->next=NULL;
			fscanf(infile,"%s",veta);
			fscanf(infile,"%c",&znak);
			fscanf(infile,"%c",&znak);
			if(znak=='(') {
				zatv++;
				i=0;
				while(zatv!=0) {
					veta[i]=znak;
					fscanf(infile,"%c",&znak);
					if(znak=='(')zatv++;
					if(znak==')')zatv--;
					i++;
				}
				veta[i]=znak;
				veta[i+1]=0;
				for(i=0;i<MAXLENGTH;i++) {
					pom_pravidlo->podmienka[i]=veta[i];
				}
			}
			fscanf(infile,"%s",veta);
			fscanf(infile,"%c",&znak);
			fscanf(infile,"%c",&znak);
			if(znak=='(') {
				zatv++;
				i=0;
				while(zatv!=0) {
					veta[i]=znak;
					fscanf(infile,"%c",&znak);
					if(znak=='(')zatv++;
					if(znak==')')zatv--;
					i++;
				}
				veta[i]=znak;
				veta[i+1]=0;
				for(i=0;i<MAXLENGTH;i++) {
					pom_pravidlo->akcia[i]=veta[i];
				}
			}
			//VLOZIM NOVE POM_PRAVIDLO DO RADU S OSTATNYMI PRAVIDLAMI
			zas_pravidiel=insert_pravidlo(pom_pravidlo,zas_pravidiel);
			fscanf(infile,"%c",&znak);
			fscanf(infile,"%c",&znak);
		}
		fclose(infile);
	}
	
	token=zas_pravidiel;
	bool flag=true;
	while(flag) {
		//VYTVORIM MNOZINU AK JE SPLENENA PODMIENKA 
		mnozina=new fakty;
		mnozina->fakt[0]=0;
		mnozina->next=NULL;
		//AKCIE NA VSETKY PRAVIDLA
		mnozina_akcii=new fakty;
		mnozina_akcii->fakt[0]=0;
		mnozina_akcii->next=NULL;
		//CYKLUS NA PRECHOD VSETKYCH PRAVIDIEL
		while(token!=NULL) {
			//EXPANDOVANY ZOZNAM FAKTOV 
			mnozina=expanduj(zas_faktov,token,mnozina,0);
			//PRIDANIE DO MNOZINY AKCII
			mnozina_akcii=insert_akcie(mnozina_akcii,mnozina);
			mnozina=new fakty;
			mnozina->fakt[0]=0;
			mnozina->next=NULL;
			token=token->next;
		}
		//KONTROLA MNOZINY NA DUPLICITNOST AKCII - DUPLICITNE ZMAZEM
		mnozina_akcii=remove_fakt(mnozina_akcii,zas_faktov);
		if(!is_empty(mnozina_akcii)) {
			//Z MNOZINY AKCII APLIKUJEM AKCIU A VYSLEDOK PRIDAM DO ZOZNAMU FAKTOV
			zas_faktov=aplikuj(mnozina_akcii,zas_faktov);
			delete mnozina;
			delete mnozina_akcii;
			token=zas_pravidiel;
			flag=true;
		}
		else flag=false;  //KONIEC
		vypis(zas_faktov);
		getchar();		 //DEBUG CAKANIE
	}
	
	return 0;
}

fakty *insert_akcie(fakty *mnozina_akcii,fakty *mnozina)
{
	fakty *i_akcii,*i;

	if(mnozina==NULL) return(mnozina_akcii);
	if(mnozina->fakt[0]==0) return(mnozina_akcii);
	if(mnozina_akcii->fakt[0]==0) return(mnozina);
	i_akcii=mnozina_akcii;
	i=mnozina;
	while(i_akcii->next!=NULL) i_akcii=i_akcii->next;
	i_akcii->next=i;

	return(mnozina_akcii);
}


//APLIKUJE PRVU AKCIU NA FAKTY
fakty *aplikuj(fakty *mnozina,fakty *zas_faktov)
{
	char buffer[40];
	int i;
	fakty *pom;
	
	sscanf(mnozina->fakt+3,"%s",buffer);
	if(!strncmp(buffer,"pridaj",6)) {
		pom=new fakty;
		pom->fakt[0]='(';
		pom->fakt[1]=' ';
		for(i=2;i<40;i++) {
			pom->fakt[i]=mnozina->fakt[i+8];
			if(pom->fakt[i]==')')break;
		}
		pom->fakt[i+1]=0;
		zas_faktov=vloz_fakt(pom,zas_faktov);
		return(zas_faktov);
	}
	else {
		pom=new fakty;
		pom->fakt[0]='(';
		pom->fakt[1]=' ';
		for(i=2;i<40;i++) {
			pom->fakt[i]=mnozina->fakt[i+7];
			if(pom->fakt[i]==')')break;
		}
		pom->fakt[i+1]=0;
		zas_faktov=vymaz(pom,zas_faktov);
	}

	return(zas_faktov);
}

//VYMAZANIE FAKTU Z MNOZINY FAKTOV
fakty *vymaz(fakty *pom,fakty *zas_faktov)
{
	fakty *token;
	token=zas_faktov;

	if(!strncmp(token->fakt,pom->fakt,strlen(pom->fakt)))
		return(token->next);
	while(strncmp(token->next->fakt,pom->fakt,strlen(pom->fakt))){token=token->next;}
	if(!strncmp(token->next->fakt,pom->fakt,strlen(pom->fakt))) {
		token->next=token->next->next;
	}

	return(zas_faktov);
}



//VYRADI AKCIE KTORE NIE SU POTREBNE
fakty *remove_fakt(fakty *mnozina,fakty *zas_faktov)
{
	char buffer[40];
	bool znova=false;	
	fakty *token,*f_pom;

	if(mnozina->fakt[0]==0) return(mnozina);
	token=mnozina;
	//PRECHOD CELOU MNOZINOU
	while(token!=NULL) {
		//NACITANIE SLOVA Z MNOZINY AKCII
		sscanf(token->fakt+3,"%s",buffer);
		//PRIDAJ
		if(!strncmp(buffer,"pridaj",6))	{
			f_pom=zas_faktov;
			//PRECHOD VSETKYMI FAKTAMI
			while(f_pom!=NULL) {
				if(!strncmp(f_pom->fakt+2,token->fakt+10,strlen(f_pom->fakt+2))) {
					//AK TAM UZ JE DANE PRAVIDLO ODSTRANIM HO A AK JE UZ MNOZINA PRAZDNA VRATIM NULL
					if((mnozina=odstran(mnozina,token)) == NULL)return(NULL);
					//NOVE PREHLADANIE MNOZINY
					token=mnozina;znova=true;break;
				}
				//INAK IDEM NA DALSI FAKT
				else f_pom=f_pom->next;
			}
			//AK SOM NIC NEZMENIL DALSI AKCIA
			if(!znova)
				token=token->next;
			znova=false;
		}
		//VYMAZ
		else if(!strncmp(buffer,"vymaz",5)) {
			f_pom=zas_faktov;
			while(f_pom!=NULL) {
				//AK NAJDEM DANY FAKY KONIEC
				if(!strncmp(f_pom->fakt+2,token->fakt+9,strlen(f_pom->fakt+2)))
				break;
				else f_pom=f_pom->next;
			}
			//PRESIEL SOM VSETKY FAKTY A NENASIEL SOM DANY, TAK HO ODSTRANIM
			if(f_pom==NULL) {
				if((mnozina=odstran(mnozina,token)) == NULL)return(NULL);
				token=mnozina;
			}
			else token=token->next;
		}
		else 
		  if(!strncmp(buffer,"sprava",6)) {
			if((mnozina=odstran(mnozina,token)) == NULL)return(NULL);
			token=mnozina;
		  }
	}

	return(mnozina);
}

//ODSTRANI AKCIU AK BOLA NEPOTREBNA
fakty *odstran(fakty *mnozina,fakty *token)
{
	fakty *pom;

	if(mnozina->fakt[0]==0)	return(mnozina);
	if(mnozina==token) {
		return(mnozina->next);
	}
	pom=mnozina;
	while((pom!=NULL)&&(pom->next!=token)) {
		pom=pom->next;
	}
	if(pom!=NULL) {
		pom->next=token->next;
	}
	delete token;

	return(mnozina);
}

//VYTVORI KOPIU PRAVIDLA 
def_pravidlo *kopiruj(def_pravidlo *pom)
{
	def_pravidlo *novy;
	int i=0;

	novy=new def_pravidlo;
	novy->next=NULL;
	for(i=0;i<13;i++) {
		novy->meno[i]=pom->meno[i];
	}
	for(i=0;i<MAXLENGTH;i++) {
		novy->podmienka[i]=pom->podmienka[i];
		novy->akcia[i]=pom->akcia[i];
	}
	novy->next=pom->next;

	return(novy);
}




fakty *expanduj(fakty *zas_faktov,def_pravidlo *pravidlo,fakty *mnozina,int start)
{
	def_pravidlo *pomocne,*zas_vymen=NULL;

	fakty *ftoken=zas_faktov;
	//X PRAVIDLO, Y POLOHA, SIZE VELKOST VYMENEHO SLOVA
	int i,x=start,y=2,size;
	char meno1[10],meno2[10];
	//V POMOCNE JE KOPIA PRAVIDLA ABY SOM SI HO NEPREPISOVAL
	pomocne=kopiruj(pravidlo);
	//PRECHOD CEZ ZASOBNIK FAKTOV
	while(ftoken!=NULL) {
		while(pomocne->podmienka[x]=='(') x++;
		while(pomocne->podmienka[x]==' ') x++;
		if(pomocne->podmienka[x]!=0) {
			i=0;
			if(ftoken->fakt[y]==' ') y++;
			//POKIAL NENAJDEM PODMIENKA[X] ALEBO OTAZNIK PRACUJEM WHILEOM
			while((ftoken->fakt[y]==pomocne->podmienka[x])||(pomocne->podmienka[x]=='?')) {
				//AK OTAZNIK UROBIM EXCHANGE NA POZICII X
				if(pomocne->podmienka[x]=='?') {
					zas_vymen=kopiruj(pomocne);
					if(!(size=exchange(zas_vymen,pomocne,ftoken,x,y))) {
						break;
					}
					//POSUN O VELKOST SLOVA
					else {x+=size;y+=size;pomocne=zas_vymen;}
				}
				//POSUN PO ZNAKOCH
				else {x++;y++;i++;}
			}
			//SOM UZ NA KONCI FAKTU
			if(y==(int)strlen(ftoken->fakt)) {	
				//V PRIPADE ZE MAM DALSIU PODMIENKU REKURZIVNE ZAVOLAM EXCHANGE
				if(pomocne->podmienka[x]=='(') {
					mnozina = expanduj(zas_faktov,kopiruj(pomocne),mnozina,x);
				}
				else 
				  if(pomocne->podmienka[x]==')') {
					//SOM NA KONCI PODMIENKY< TAK PRIDAM AKCIU DO MNOZINY
					mnozina = vloz_mnozina(pomocne->akcia,mnozina);
				}
				else {
					printf("\n Nespravny ukoncovaci znak %c %d:",pomocne->podmienka[x],x);
					//PROBLEM S KONCOM
				}
			}
			else 
			  if(pomocne->podmienka[x]=='<') {		//SPECIAL PODMIENKA	
				//POSUN ZNAKU
				x+=3;
				//NACITANIE MENA
				sscanf(pomocne->podmienka+x,"%s",meno1);
				sscanf(pomocne->podmienka+x+strlen(meno1),"%s",meno2);
				//AK SA ROVNAJU NIC NEROB
				if(strcmp(meno1,meno2) == 0) return(mnozina);
				//AK ROZNE PRIDAJ AKCIU DO MNOZINY
				return((mnozina = vloz_mnozina(pomocne->akcia,mnozina)));
			}
			//OBNOVIM SI POKAZENE PRAVIDLO
			pomocne=kopiruj(pravidlo);
			//SEEK NA ZACIATOK A IDEM SPRACOVAT DALSI FAKT
			y=2;x=start;
			ftoken=ftoken->next;
		}
	}

	return(mnozina);
}

//VLOZENIE NOVEHO FAKTU-TEXTU DO MNOZINY
fakty *vloz_mnozina(char *text,fakty *mnozina)
{
	fakty *pom,*token;
	pom=new fakty;
	int i;

	for(i=0;i<40;i++) {
		pom->fakt[i]=text[i];
	}
	pom->next=NULL;
	if(mnozina->fakt[0]==0) mnozina=pom;
	else {
		token=mnozina;
		while(token->next!=NULL) token=token->next;
		token->next=pom;
	}

	return(mnozina);
}


//VYMIENA = NAVAZUJE PREMENNE
int exchange(def_pravidlo *zas_vymen,def_pravidlo *pomocne,fakty *ftoken,int x,int y)
{
	
	//do premenna ?X, do meno ide meno ktora sa ma nahrasit za ?X, slovo pomocna
	char premenna[5],meno[15],slovo[15],checkin;
	int i,j,v_mena=0,v_premennej=0,v_slova=0;

	meno[0]='\0';
	//PREMENNA, KTORU IDEM NAHRADZOVAT
	sscanf(zas_vymen->podmienka+x,"%s",premenna);
	v_premennej=strlen(premenna);
	if((ftoken->fakt[y]!='<')&&(ftoken->fakt[y]<='Z')) {	
			sscanf(ftoken->fakt+y,"%s",meno);
			v_mena=strlen(meno);
			y=x;
			//PREJDEM CEZ CELU PODMIENKU A UROBIM ZAMENU ZA PREMENNU MENOM
			while(sscanf(zas_vymen->podmienka+x,"%s",slovo)!=0) {
				v_slova=strlen(slovo);
				if((v_premennej==v_slova)&&(!strncmp(premenna,slovo,v_premennej))) {
					i=x;j=0;
					//V ZAS_VYMENA MENO
					for(i=i;i<(x+v_mena);i++,j++) {
						zas_vymen->podmienka[i]=meno[j];
					}
					zas_vymen->podmienka[i]='\0';
					//DOPLNI ZVYSOK PODMIENKY
					strcat(zas_vymen->podmienka,pomocne->podmienka+y+v_premennej);
					//SEEK
					x+=1+v_mena;
					y+=1+v_premennej;
				}
				else {  	//AK TAM NIE JE POSUNIEM SA V OBOCH O SLOVO
					x+=(1+v_slova);
					y+=(1+v_slova);
				}
				checkin=zas_vymen->podmienka[x-1];
				if(checkin=='\0')break;
			}
			//SPRACOVANIE ACKIE
			x=3; y=3;
			while(sscanf(zas_vymen->akcia+x,"%s",slovo)!=0) {
				v_slova=strlen(slovo);
				if((v_premennej==v_slova)&&(!strncmp(premenna,slovo,v_premennej))) {
					i=x;j=0;
					for(i=i;i<(x+v_mena);i++,j++) {
						zas_vymen->akcia[i]=meno[j];
					}
					zas_vymen->akcia[i]='\0';
					strcat(zas_vymen->akcia,pomocne->akcia+y+v_premennej);
					x+=1+v_mena;
					y+=1+v_premennej;
				}
				else {
					x+=(1+v_slova);
					y+=(1+v_slova);
				}
				checkin=zas_vymen->akcia[x-1];
				if(checkin==0) break;
			}
		//0 AK NEDOSLO K NAHRADZANIU
		}
	    else return(0);
		//VELKOST MENA< KTORU SOM NAHRADZAL
		return(v_mena);
}

//VYPIS ZASOBNIKA FAKTOV
void vypis(fakty *zas_faktov)
{
	fakty *token;
	token=zas_faktov;

	printf("\n");
	while(token!=NULL) {
		printf(" %s\n",token->fakt);
		token=token->next;
	}
}



//URCI CI JE MNOZINA PRAZDNA
bool is_empty(fakty *mnozina)
{
	if(mnozina==NULL)return(true);
	if (mnozina->fakt[0]==0) return(true);
	return(false);
}


//VKLADA PRAVIDLO DO ZASOBNIKA PRAVIDIEL
def_pravidlo *insert_pravidlo(def_pravidlo *pom,def_pravidlo *zas_pravidiel)
{
	def_pravidlo *novy,*token;
	int i=0;

	novy=new def_pravidlo;
	novy->next=NULL;
	for(i=0;i<40;i++) {
		novy->meno[i]=pom->meno[i];
	}
	for(i=0;i<MAXLENGTH;i++) {
		novy->podmienka[i]=pom->podmienka[i];
		novy->akcia[i]=pom->akcia[i];
	}
	token=zas_pravidiel;
	if(zas_pravidiel->meno[0]==0)	{
		zas_pravidiel=novy;
	}
	else {
		while(token->next!=NULL){token=token->next;}
		token->next=novy;
		
	}

	return(zas_pravidiel);

}




//VKLADA FAKT DO ZASOBNIKA FAKTOV
fakty *vloz_fakt(fakty *pom,fakty *zas_faktov)
{
	fakty *novy,*token;
	int i=0;

	novy=new fakty;
	novy->next=NULL;
	for(i=0;i<40;i++) {
		novy->fakt[i]=pom->fakt[i];
	}
	token=zas_faktov;
	if(zas_faktov->fakt[0]==0) {
		zas_faktov=novy;
	}
	else {
		while(token->next!=NULL){token=token->next;}
		token->next=novy;
	}

	return(zas_faktov);
}
