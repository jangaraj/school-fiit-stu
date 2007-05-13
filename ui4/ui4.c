#include <stdio.h>
#include <string.h>
#include "struktury.h"

ppravidla *Kopia(ppravidla *pom);
ppravidla *Vloz_pravidlo(ppravidla *pom,ppravidla *z_pravidiel);
fakty *Vyrad(fakty *mnozina,fakty *z_faktov);
fakty *Aplikuj(fakty *mnozina,fakty *z_pravidiel);
fakty *Expand(fakty *z_faktov,ppravidla *z_pravidiel,fakty *mnozina,int x);
fakty *Pridaj_akcie(fakty *mnozina_akcii,fakty *mnozina);
fakty *Vloz(fakty *pom,fakty *z_faktov);
fakty *Vloz_mnozina(char *text,fakty *mnozina); 
int Vymena(ppravidla *z_vymeny,ppravidla *pomocne,fakty *fiterator,int x,int y);
void vypis(fakty *z_faktov);
bool IsEmpty(fakty *mnozina);


int main(void)
{
	fakty *z_faktov;
	fakty *pom_fakt;
	ppravidla *z_pravidiel;
	ppravidla *pom_pravidlo;
	z_faktov=new fakty;
	z_faktov->fakt[0]=0;
	z_faktov->next=NULL;
	z_pravidiel=new ppravidla;
	z_pravidiel->next=NULL;
	z_pravidiel->meno[0]=0;
	z_pravidiel->podmienka[0]=0;
	z_pravidiel->akcia[0]=0;
	char veta[80];
	char znak=0;
	int i=0;
	int zatvorka=0;
	FILE *infile;

	/* NACITANIE FAKTOV */
	if((infile=fopen("fakty.txt","rd")) == NULL ) {
		fprintf(stderr,"Error, opening fakty.txt\n");
		return 1;
	}
	else {
		while(fscanf(infile,"%c",&znak)!=EOF) {
			i=0;
			if(znak=='(') {
				zatvorka++;
				while(zatvorka!=0) {
					veta[i]=znak;
					fscanf(infile,"%c",&znak);
					if(znak=='(')zatvorka++;
					if(znak==')')zatvorka--;
					i++;
				}
				veta[i]=znak;
				veta[i+1]=0;
				pom_fakt=new fakty;
				for(i=0;i<40;i++) {
					pom_fakt->fakt[i]=veta[i];
				}
				pom_fakt->next=NULL;
				z_faktov=Vloz(pom_fakt,z_faktov);
			}
		}
		fclose(infile);
	}
	/* NACITANIE PRAVIDIEL */

	zatvorka=0;
	if((infile=fopen("pravidla.txt","rd")) == NULL) {
		fprintf(stderr,"Error, opening pravidla.txt\n");
		return 1;
	}
	else {
		while(fscanf(infile,"%c",&znak)!=EOF) {
			i=0;
			pom_pravidlo=new ppravidla;
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
				zatvorka++;
				i=0;
				while(zatvorka!=0) {
					veta[i]=znak;
					fscanf(infile,"%c",&znak);
					if(znak=='(')zatvorka++;
					if(znak==')')zatvorka--;
					i++;
				}
				veta[i]=znak;
				veta[i+1]=0;
				for(i=0;i<80;i++) {
					pom_pravidlo->podmienka[i]=veta[i];
				}
			}
			fscanf(infile,"%s",veta);
			fscanf(infile,"%c",&znak);
			fscanf(infile,"%c",&znak);
			if(znak=='(') {
				zatvorka++;
				i=0;
				while(zatvorka!=0) {
					veta[i]=znak;
					fscanf(infile,"%c",&znak);
					if(znak=='(')zatvorka++;
					if(znak==')')zatvorka--;
					i++;
				}
				veta[i]=znak;
				veta[i+1]=0;
				for(i=0;i<80;i++) {
					pom_pravidlo->akcia[i]=veta[i];
				}
			}
			z_pravidiel=Vloz_pravidlo(pom_pravidlo,z_pravidiel);
			fscanf(infile,"%c",&znak);
			fscanf(infile,"%c",&znak);
		}
		fclose(infile);
	}
	fakty *mnozina,*mnozina_akcii;
	ppravidla *iterator=z_pravidiel;
	bool test=true;
	while(test) {
		// vytvori sa mnozina na ukladanie ked sa poddari splnit podmienku, tak tam ulozim akciu -|> za potom
		mnozina=new fakty;
		mnozina->fakt[0]=0;
		mnozina->next=NULL;
		// akcie pre vsetky pravidla
		mnozina_akcii=new fakty;
		mnozina_akcii->fakt[0]=0;
		mnozina_akcii->next=NULL;
		// prejdem vsetky pravidla
		while(iterator!=NULL) {
			// zoznam faktov, pravidlo, prve, druhe .., do mnozina sa pridaju akcie, 0 -> som na nultom znaku 
			mnozina=Expand(z_faktov,iterator,mnozina,0);
			// pridam do mn akcii
			mnozina_akcii=Pridaj_akcie(mnozina_akcii,mnozina);
			mnozina=new fakty;
			mnozina->fakt[0]=0;
			mnozina->next=NULL;
			iterator=iterator->next;
		}
		// prejdem celu mnozinu akciia vyhodim take ktora tam uz su a ma ich pridat, alebo vymazem take ktore mam vymazat a tam niesu ..
		mnozina_akcii=Vyrad(mnozina_akcii,z_faktov);
		// ake je prazdna tak konec, ak nie je
		if(!IsEmpty(mnozina_akcii)) {
			//z mnozi ny akcii pridam prve pravidlo do zoznamu faktov
			z_faktov=Aplikuj(mnozina_akcii,z_faktov);
			delete mnozina;
			delete mnozina_akcii;
			// zoznam faktov vecsi o 1
			iterator=z_pravidiel;
			test=true;
		}
		else test=false;
		vypis(z_faktov);
		getchar();
	}
	
	return 0;
}

/* vola sa pouziva aby poskladala z mnoziny prida vsetky akcie do mnoziny akcii
 vola sa to v maine, mnoz_akcii -> vsetky */
fakty *Pridaj_akcie(fakty *mnozina_akcii,fakty *mnozina)
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

fakty *Vymaz(fakty *pom,fakty *z_faktov);

// aplikuje hned prvu akciu z tej velkej na mnozinu faktov (aj aplikuj aj vyrad z tej velkej)
fakty *Aplikuj(fakty *mnozina,fakty *z_faktov)
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
		z_faktov=Vloz(pom,z_faktov);
		return(z_faktov);
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
		z_faktov=Vymaz(pom,z_faktov);
	}

	return(z_faktov);
}

// vymaz fakt z faktov
fakty *Vymaz(fakty *pom,fakty *z_faktov)
{
	fakty *iterator;
	iterator=z_faktov;

	if(!strncmp(iterator->fakt,pom->fakt,strlen(pom->fakt)))
		return(iterator->next);
	while(strncmp(iterator->next->fakt,pom->fakt,strlen(pom->fakt))){iterator=iterator->next;}
	if(!strncmp(iterator->next->fakt,pom->fakt,strlen(pom->fakt))) {
		iterator->next=iterator->next->next;
	}

	return(z_faktov);
}


fakty *Odstran(fakty *mnozina,fakty *iterator);

// vyradi akcie ktore netreba
// v mnozina hlada akcie, ktore z nej odstrani ak su nepotrebne, ak su v zozname faktov, atd ...
// ak mam pridat to co uz tam je ...alebo vymazat to co tam neni ci je ci co ...
fakty *Vyrad(fakty *mnozina,fakty *z_faktov)
{
	fakty *iterator,*preh;
	char buffer[40];
	bool odznova=false;	

	if(mnozina->fakt[0]==0) return(mnozina);
	iterator=mnozina;
	// prejdem celu mozinu akcii
	while(iterator!=NULL) {
		// nacitam slovo z mnoziny akcii
		sscanf(iterator->fakt+3,"%s",buffer);
		// ak to je pridaj
		if(!strncmp(buffer,"pridaj",6))	{
			preh=z_faktov;
			// prejde cez vsetky fakty
			while(preh!=NULL) {
				//ak tam take previdlo uz je
				if(!strncmp(preh->fakt+2,iterator->fakt+10,strlen(preh->fakt+2))) {
					//odstranim ho z mnoziny, ak uz je mnozina prazdna tak vratim null
					if((mnozina=Odstran(mnozina,iterator)) == NULL)return(NULL);
					//prehladavam znova celu mnozinu ak som ju zmenil
					iterator=mnozina;odznova=true;break;
				}
				// ak sa nerovna idem na dalsi fakt
				else preh=preh->next;
			}
			// ak som nic nezmenil idem na dalsiu akciu
			if(!odznova)
				iterator=iterator->next;
			odznova=false;
		}
		// ak vymaz
		else if(!strncmp(buffer,"vymaz",5)) {
			preh=z_faktov;
			// prejde fekty
			while(preh!=NULL) {
				// ak ho najdem brejk
				if(!strncmp(preh->fakt+2,iterator->fakt+9,strlen(preh->fakt+2)))
				break;
				else preh=preh->next;
			}
			// ak som presiel vsetky a nenasiel somtaky coo treba vymzata tak akciu odstranim
			if(preh==NULL) {
				if((mnozina=Odstran(mnozina,iterator)) == NULL)return(NULL);
				iterator=mnozina;
			}
			else iterator=iterator->next;
		}
		else 
		  if(!strncmp(buffer,"sprava",6)) {
			if((mnozina=Odstran(mnozina,iterator)) == NULL)return(NULL);
			iterator=mnozina;
		  }
	}

	return(mnozina);
}

// odstranuje akciu ak bola nepotrebna
// vola sa vo vyrad, z ktorej mnoziny a druhy co sa ma vymazat
fakty *Odstran(fakty *mnozina,fakty *iterator)
{
	fakty *pom;

	if(mnozina->fakt[0]==0)return(mnozina);
	if(mnozina==iterator) {
		return(mnozina->next);
	}
	pom=mnozina;
	while((pom!=NULL)&&(pom->next!=iterator)) {
		pom=pom->next;
	}
	if(pom!=NULL) {
		pom->next=iterator->next;
	}
	delete iterator;

	return(mnozina);
}

// robi kopiu pravidla v expande 
ppravidla *Kopia(ppravidla *pom)
{
	ppravidla *novy;
	int i=0;

	novy=new ppravidla;
	novy->next=NULL;
	for(i=0;i<13;i++) {
		novy->meno[i]=pom->meno[i];
	}
	for(i=0;i<80;i++) {
		novy->podmienka[i]=pom->podmienka[i];
		novy->akcia[i]=pom->akcia[i];
	}
	novy->next=pom->next;

	return(novy);
}




// rek funkcia, zoz fakt, jednmo pravidlo, do mnoz pridavam
fakty *Expand(fakty *z_faktov,ppravidla *pravidlo,fakty *mnozina,int start)
{
	// do pomocne si ulozim to pravidlo aby som si to neprepisoval
	// z_vymeny
	ppravidla *pomocne,*z_vymeny=NULL;

	// prejde cez vsetky fakty
	fakty *fiterator=z_faktov;
	// x poloha v pravidle, y poloha vo fakte, size, velkost vymeneneho slova jano = 4
	int i,x=start,y=2,size;
	// rozne ... brat toho a toho
	char meno1[10],meno2[10];
	// vytvori kopiu
	pomocne=Kopia(pravidlo);
	// prejdem zoz faktov
	while(fiterator!=NULL) {
		while(pomocne->podmienka[x]=='(')x++;
		while(pomocne->podmienka[x]==' ')x++;
		// ka ma pravidlo podmienku
		if(pomocne->podmienka[x]!=0) {
			i=0;
			// preskocim medzery vo faktoch
			if(fiterator->fakt[y]==' ') y++;
			// kkym sa rovna taext a lebo otaznik
			while((fiterator->fakt[y]==pomocne->podmienka[x])||(pomocne->podmienka[x]=='?')) {
				//ak otaznik
				if(pomocne->podmienka[x]=='?') {
					z_vymeny=Kopia(pomocne);
//					if(fiterator==NULL)return(NULL); // nenasiel nijake vhodne
					//vracia velkost vymeneho slova
					//z_vymeny a pomocne -> pravidlo, fiterator je aktualny fakt
					//posuniem sa na poz x, vlozim slovo z faktu(fiterator) a zvyskok doplnim z pomocneho
					// ak nezamenil ... velkost bola nula
					if(!(size=Vymena(z_vymeny,pomocne,fiterator,x,y))) {
/*						fiterator=fiterator->next;
						y=2; x=start;
						pomocne=Kopia(pravidlo);
*/						break;
					}
					// posuniem sa o velkost slova a ulozim opet do pomocneho
					else {x+=size;y+=size;pomocne=z_vymeny;}
				}
				// posuvanie po znakoch akkrovnake
				else {x++;y++;i++;}
			}
			// ak som na konci faktu
			if(y==(int)strlen(fiterator->fakt)) {	
				// ak mam dalsiu podmienku
				if(pomocne->podmienka[x]=='(') {
					// rekurzivne idem robit dalsiu ... zoz faktov, kopia upraveneho (prve uz nahradene), mnozi na do ktorej ukladam, pozicia
					mnozina = Expand(z_faktov,Kopia(pomocne),mnozina,x);
				}
				// ak som na konci podmienky
				else 
				  if(pomocne->podmienka[x]==')') {
					// pridam akciu do mnoziny
					mnozina = Vloz_mnozina(pomocne->akcia,mnozina);
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
				return((mnozina = Vloz_mnozina(pomocne->akcia,mnozina)));
			}
			// z brejku sem -|> fakt sa nehodi pre pravidlo
			// obnovim pravdlo
			pomocne=Kopia(pravidlo);
			//nastavim zac poz a posuniem sa na dalsi fakt
			y=2;x=start;
			fiterator=fiterator->next;
		}
	}

	return(mnozina);
}

// ked expand nasiel vysledok, podarilo sa mu splnit podmienkovu cast tak akciu pridal do mnoziny
// text akcia, do mnoziny vkladam
fakty *Vloz_mnozina(char *text,fakty *mnozina)
{
	fakty *pom,*iterator;
	pom=new fakty;
	int i;

	for(i=0;i<40;i++) {
		pom->fakt[i]=text[i];
	}
	pom->next=NULL;
	if(mnozina->fakt[0]==0) mnozina=pom;
	else {
		iterator=mnozina;
		while(iterator->next!=NULL) iterator=iterator->next;
		iterator->next=pom;
	}

	return(mnozina);
}


// vymiena .. namiesto otaznika da konstantu z faktov ... naviaze premennu 
// do z_vym sa uklada ...
int Vymena(ppravidla *z_vymeny,ppravidla *pomocne,fakty *fiterator,int x,int y)
{
	
	// do premenna ?X, do meno ide meno ktora sa ma nahrasit za ?X, slovo pomocna
	char premenna[5],meno[15],slovo[15],kontrolny;
	// v_mena velkost mena, ...
	int i,j,v_mena=0,v_premennej=0,v_slova=0;
//	z_vymeny=Kopia(pomocne);
	meno[0]='\0';
	// nacita sa premenna co sa ide nahradzovat
	sscanf(z_vymeny->podmienka+x,"%s",premenna);
	// velkost prem
	v_premennej=strlen(premenna);
	// mesmie to byt <> a musi to byt velkyt znak, teda meno, nacitava sa z faktu
	if((fiterator->fakt[y]!='<')&&(fiterator->fakt[y]<='Z')) {	
			// y z faktu      ... z faktu nacitam meno
			sscanf(fiterator->fakt+y,"%s",meno);
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
void vypis(fakty *z_faktov)
{
	fakty *iterator;
	iterator=z_faktov;

	printf("\n");
	while(iterator!=NULL) {
		printf(" %s\n",iterator->fakt);
		iterator=iterator->next;
	}
}



// ci je prazdna mnozina
bool IsEmpty(fakty *mnozina)
{
	if(mnozina==NULL)return(true);
	if (mnozina->fakt[0]==0) return(true);
	return(false);
}


// pri nacit suborov, pouziva sa na vytvaranie pravidla, pravidlo ktore idem vlozit a kam vkladam
ppravidla *Vloz_pravidlo(ppravidla *pom,ppravidla *z_pravidiel)
{
	ppravidla *novy,*iterator;
	int i=0;

	novy=new ppravidla;
	novy->next=NULL;
	for(i=0;i<40;i++) {
		novy->meno[i]=pom->meno[i];
	}
	for(i=0;i<80;i++) {
		novy->podmienka[i]=pom->podmienka[i];
		novy->akcia[i]=pom->akcia[i];
	}
	iterator=z_pravidiel;
	if(z_pravidiel->meno[0]==0)	{
		z_pravidiel=novy;
	}
	else {
		while(iterator->next!=NULL){iterator=iterator->next;}
		iterator->next=novy;
		
	}

	return(z_pravidiel);

}




// vklada novy fakt pom  do zoznamu faktov, vola sa pri aplikuj a nacitani suboru
fakty *Vloz(fakty *pom,fakty *z_faktov)
{
	fakty *novy,*iterator;
	int i=0;

	novy=new fakty;
	novy->next=NULL;
	for(i=0;i<40;i++) {
		novy->fakt[i]=pom->fakt[i];
	}
	iterator=z_faktov;
	if(z_faktov->fakt[0]==0) {
		z_faktov=novy;
	}
	else {
		while(iterator->next!=NULL){iterator=iterator->next;}
		iterator->next=novy;
	}

	return(z_faktov);
}


