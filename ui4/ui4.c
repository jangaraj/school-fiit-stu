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
fakty *Vloz(fakty *pom,fakty *zas_faktov);
fakty *vloz_mnozina(char *text,fakty *mnozina);
fakty *vymaz(fakty *pom,fakty *zas_faktov);
int exchange(def_pravidlo *z_vymeny,def_pravidlo *pomocne,fakty *ftoken,int x,int y);
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
				zas_faktov=Vloz(pom_fakt,zas_faktov);
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
	bool test=true;
	while(test) {
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
			//z mnozi ny akcii pridam prve pravidlo do zoznamu faktov
			zas_faktov=aplikuj(mnozina_akcii,zas_faktov);
			delete mnozina;
			delete mnozina_akcii;
			// zoznam faktov vecsi o 1
			token=zas_pravidiel;
			test=true;
		}
		else test=false;  //KONIEC
		vypis(zas_faktov);
		getchar();
	}
	
	return 0;
}

/* vola sa pouziva aby poskladala z mnoziny prida vsetky akcie do mnoziny akcii
 vola sa to v maine, mnoz_akcii -> vsetky */
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


// aplikuje hned prvu akciu z tej velkej na mnozinu faktov (aj aplikuj aj vyrad z tej velkej)
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
		zas_faktov=Vloz(pom,zas_faktov);
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

// vymaz fakt z faktov
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


fakty *Odstran(fakty *mnozina,fakty *token);

// vyradi akcie ktore netreba
// v mnozina hlada akcie, ktore z nej odstrani ak su nepotrebne, ak su v zozname faktov, atd ...
// ak mam pridat to co uz tam je ...alebo vymazat to co tam neni ci je ci co ...
fakty *remove_fakt(fakty *mnozina,fakty *zas_faktov)
{
	fakty *token,*preh;
	char buffer[40];
	bool odznova=false;	

	if(mnozina->fakt[0]==0) return(mnozina);
	token=mnozina;
	// prejdem celu mozinu akcii
	while(token!=NULL) {
		// nacitam slovo z mnoziny akcii
		sscanf(token->fakt+3,"%s",buffer);
		// ak to je pridaj
		if(!strncmp(buffer,"pridaj",6))	{
			preh=zas_faktov;
			// prejde cez vsetky fakty
			while(preh!=NULL) {
				//ak tam take previdlo uz je
				if(!strncmp(preh->fakt+2,token->fakt+10,strlen(preh->fakt+2))) {
					//odstranim ho z mnoziny, ak uz je mnozina prazdna tak vratim null
					if((mnozina=Odstran(mnozina,token)) == NULL)return(NULL);
					//prehladavam znova celu mnozinu ak som ju zmenil
					token=mnozina;odznova=true;break;
				}
				// ak sa nerovna idem na dalsi fakt
				else preh=preh->next;
			}
			// ak som nic nezmenil idem na dalsiu akciu
			if(!odznova)
				token=token->next;
			odznova=false;
		}
		// ak vymaz
		else if(!strncmp(buffer,"vymaz",5)) {
			preh=zas_faktov;
			// prejde fekty
			while(preh!=NULL) {
				// ak ho najdem brejk
				if(!strncmp(preh->fakt+2,token->fakt+9,strlen(preh->fakt+2)))
				break;
				else preh=preh->next;
			}
			// ak som presiel vsetky a nenasiel somtaky coo treba vymzata tak akciu odstranim
			if(preh==NULL) {
				if((mnozina=Odstran(mnozina,token)) == NULL)return(NULL);
				token=mnozina;
			}
			else token=token->next;
		}
		else 
		  if(!strncmp(buffer,"sprava",6)) {
			if((mnozina=Odstran(mnozina,token)) == NULL)return(NULL);
			token=mnozina;
		  }
	}

	return(mnozina);
}

// odstranuje akciu ak bola nepotrebna
// vola sa vo vyrad, z ktorej mnoziny a druhy co sa ma vymazat
fakty *Odstran(fakty *mnozina,fakty *token)
{
	fakty *pom;

	if(mnozina->fakt[0]==0)return(mnozina);
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

// robi kopiu pravidla v expande 
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




// rekurzivna funkcia, zober fakt, jedno pravidlo, do mnoz pridavam
fakty *expanduj(fakty *zas_faktov,def_pravidlo *pravidlo,fakty *mnozina,int start)
{
	// do pomocne si ulozim to pravidlo aby som si to neprepisoval
	// z_vymeny
	def_pravidlo *pomocne,*z_vymeny=NULL;

	// prejde cez vsetky fakty
	fakty *ftoken=zas_faktov;
	// x poloha v pravidle, y poloha vo fakte, size, velkost vymeneneho slova jano = 4
	int i,x=start,y=2,size;
	// rozne ... brat toho a toho
	char meno1[10],meno2[10];
	// vytvori kopiu
	pomocne=kopiruj(pravidlo);
	// prejdem zoz faktov
	while(ftoken!=NULL) {
		while(pomocne->podmienka[x]=='(')x++;
		while(pomocne->podmienka[x]==' ')x++;
		// ka ma pravidlo podmienku
		if(pomocne->podmienka[x]!=0) {
			i=0;
			// preskocim medzery vo faktoch
			if(ftoken->fakt[y]==' ') y++;
			// kkym sa rovna taext a lebo otaznik
			while((ftoken->fakt[y]==pomocne->podmienka[x])||(pomocne->podmienka[x]=='?')) {
				//ak otaznik
				if(pomocne->podmienka[x]=='?') {
					z_vymeny=kopiruj(pomocne);
//					if(ftoken==NULL)return(NULL); // nenasiel nijake vhodne
					//vracia velkost vymeneho slova
					//z_vymeny a pomocne -> pravidlo, ftoken je aktualny fakt
					//posuniem sa na poz x, vlozim slovo z faktu(ftoken) a zvyskok doplnim z pomocneho
					// ak nezamenil ... velkost bola nula
					if(!(size=exchange(z_vymeny,pomocne,ftoken,x,y))) {
/*						ftoken=ftoken->next;
						y=2; x=start;
						pomocne=kopiruj(pravidlo);
*/						break;
					}
					// posuniem sa o velkost slova a ulozim opet do pomocneho
					else {x+=size;y+=size;pomocne=z_vymeny;}
				}
				// posuvanie po znakoch akkrovnake
				else {x++;y++;i++;}
			}
			// ak som na konci faktu
			if(y==(int)strlen(ftoken->fakt)) {	
				// ak mam dalsiu podmienku
				if(pomocne->podmienka[x]=='(') {
					// rekurzivne idem robit dalsiu ... zoz faktov, kopia upraveneho (prve uz nahradene), mnozi na do ktorej ukladam, pozicia
					mnozina = expanduj(zas_faktov,kopiruj(pomocne),mnozina,x);
				}
				// ak som na konci podmienky
				else 
				  if(pomocne->podmienka[x]==')') {
					// pridam akciu do mnoziny
					mnozina = vloz_mnozina(pomocne->akcia,mnozina);
				}
				else {
					// k nieco ine ...
					printf("\n Nespravny ukoncovaci znak %c %d:",pomocne->podmienka[x],x);
				}
			}
			// ak nhajdem znak <> 
			else 
			  if(pomocne->podmienka[x]=='<') {
				// posun sa o znak
				x+=3;
				// nazitaj mena
				sscanf(pomocne->podmienka+x,"%s",meno1);
				sscanf(pomocne->podmienka+x+strlen(meno1),"%s",meno2);
				// ak su rovnake vraciam mnozinu bez zmeny
				if(strcmp(meno1,meno2) == 0) return(mnozina);
				// inak pridam akciu a vratim mnozinu s akciiou
				return((mnozina = vloz_mnozina(pomocne->akcia,mnozina)));
			}
			// z brejku sem -|> fakt sa nehodi pre pravidlo
			// obnovim pravdlo
			pomocne=kopiruj(pravidlo);
			//nastavim zac poz a posuniem sa na dalsi fakt
			y=2;x=start;
			ftoken=ftoken->next;
		}
	}

	return(mnozina);
}

// ked expand nasiel vysledok, podarilo sa mu splnit podmienkovu cast tak akciu pridal do mnoziny
// text akcia, do mnoziny vkladam
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


// vymiena .. namiesto otaznika da konstantu z faktov ... naviaze premennu 
// do z_vym sa uklada ...
int exchange(def_pravidlo *z_vymeny,def_pravidlo *pomocne,fakty *ftoken,int x,int y)
{
	
	// do premenna ?X, do meno ide meno ktora sa ma nahrasit za ?X, slovo pomocna
	char premenna[5],meno[15],slovo[15],kontrolny;
	// v_mena velkost mena, ...
	int i,j,v_mena=0,v_premennej=0,v_slova=0;
//	z_vymeny=kopiruj(pomocne);
	meno[0]='\0';
	// nacita sa premenna co sa ide nahradzovat
	sscanf(z_vymeny->podmienka+x,"%s",premenna);
	// velkost prem
	v_premennej=strlen(premenna);
	// mesmie to byt <> a musi to byt velkyt znak, teda meno, nacitava sa z faktu
	if((ftoken->fakt[y]!='<')&&(ftoken->fakt[y]<='Z')) {	
			// y z faktu      ... z faktu nacitam meno
			sscanf(ftoken->fakt+y,"%s",meno);
			v_mena=strlen(meno);
			// y na fakt, y ma novu funkciu, bude tam pozicia zo zdrojoveho previdla
			y=x; /// x je z cieloveho pravidla  y bude zo zdrojoveho pravidla
			// prejde cez celu podmienku a nahradom vsetky vyskyty pewmennej menom
			// nacitam slovo a porovnavam s premennou
			while(sscanf(z_vymeny->podmienka+x,"%s",slovo)!=0) {
				v_slova=strlen(slovo);
				// ak prva nemusi byt, ak premenna a slovo su rovnake, treba na hradit slovo menom
				if((v_premennej==v_slova)&&(!strncmp(premenna,slovo,v_premennej))) {
					i=x;j=0;
					// do z_vymeny zacnem pisat meno od pozicie premennej
					for(i=i;i<(x+v_mena);i++,j++) {
						z_vymeny->podmienka[i]=meno[j];
					}
					// vymeni a da nulu
					z_vymeny->podmienka[i]='\0';
					//pripoji zvysok celej podmienky
					strcat(z_vymeny->podmienka,pomocne->podmienka+y+v_premennej);
					// posuniem sa v cielovej podmienke o meno a v zdrojovej o ?X
					x+=1+v_mena;
					y+=1+v_premennej;
				}
				// ak to neni premenna tak v oboch o slovo
				else {
					x+=(1+v_slova);
					y+=(1+v_slova);
				}
				kontrolny=z_vymeny->podmienka[x-1];
				if(kontrolny=='\0')break;
			}
			// to iste ale na akcii
			x=3; y=3;
			while(sscanf(z_vymeny->akcia+x,"%s",slovo)!=0) {
				v_slova=strlen(slovo);
				if((v_premennej==v_slova)&&(!strncmp(premenna,slovo,v_premennej))) {
					i=x;j=0;
					for(i=i;i<(x+v_mena);i++,j++) {
						z_vymeny->akcia[i]=meno[j];
					}
					z_vymeny->akcia[i]='\0';
					strcat(z_vymeny->akcia,pomocne->akcia+y+v_premennej);
					x+=1+v_mena;
					y+=1+v_premennej;
				}
				else {
					x+=(1+v_slova);
					y+=(1+v_slova);
				}
				kontrolny=z_vymeny->akcia[x-1];
				if(kontrolny==0) break;
			}
		// vratim 0 ak sa nenahradzalo
		}
	    else return(0);
		// vratim velkost mena ak sa nahradzalo
		return(v_mena);
}

// vypis ...
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



// ci je prazdna mnozina
bool is_empty(fakty *mnozina)
{
	if(mnozina==NULL)return(true);
	if (mnozina->fakt[0]==0) return(true);
	return(false);
}


// pri nacit suborov, pouziva sa na vytvaranie pravidla, pravidlo ktore idem vlozit a kam vkladam
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




// vklada novy fakt pom  do zoznamu faktov, vola sa pri aplikuj a nacitani suboru
fakty *Vloz(fakty *pom,fakty *zas_faktov)
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


