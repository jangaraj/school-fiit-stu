#include <stdio.h>
#include <stdlib.h>
#include "front.h"
#include "prvok_strom.h"

void front_insert(prvok_strom *c, front *p_front) {
	prvok_front *pom;
	if((pom=(prvok_front *) malloc(sizeof(prvok_front)))==NULL) { printf("Nedostatok pamate pri vkladani prvku do fronty\n"); exit(1); };
	pom->c=(void *) c;	//prekopirovanie ukazovatela
	pom->next = NULL;
	if (p_front->in != NULL) p_front->in->next = pom;		//ak front obsahuje aspon jeden prvok, nastavime u posledneho prvku ukazovatel next na novovytvoreny prvok
	p_front->in = pom;
	if (p_front->out == NULL) p_front->out = pom;		//ak sme do prazdneho frontu pridali prvok, je to zaroven
}

void front_insert2(stav_s_historiou *c, front *p_front) {
	prvok_front *pom;
	if((pom=(prvok_front *) malloc(sizeof(prvok_front)))==NULL) { printf("Nedostatok pamate pri vkladani prvku do fronty\n"); exit(1); };
	pom->c=(void *) c;	//prekopirovanie ukazovatela
	pom->next = NULL;
	if (p_front->in != NULL) p_front->in->next = pom;		//ak front obsahuje aspon jeden prvok, nastavime u posledneho prvku ukazovatel next na novovytvoreny prvok
	p_front->in = pom;
	if (p_front->out == NULL) p_front->out = pom;		//ak sme do prazdneho frontu pridali prvok, je to zaroven
}


//Vratenie a odstranenie prvku z fronty
prvok_strom *front_get(front *p_front) {
	if (p_front->out == NULL) return NULL;
	prvok_strom *pom = (prvok_strom *)p_front->out->c;	//ulozenie hodnoty vyberaneho prvku fronty (ukazovatela na objekt
	prvok_front *pom_p = p_front->out;	   				//ulozenie smernika na vyberany prvok fronty
	p_front->out = p_front->out->next;
	if (p_front->out == NULL) p_front->in = NULL;		//ak sme odstranili posledny prvok a front zostal prazdny,
	//vynulujeme ukazovatel na prvy prvok
	free((void *) pom_p);
	return pom;
}

stav_s_historiou *front_get2(front *p_front) {
	if (p_front->out == NULL) return NULL;
	stav_s_historiou *pom = (stav_s_historiou *)p_front->out->c;	//ulozenie hodnoty vyberaneho prvku fronty (ukazovatela na objekt
	prvok_front *pom_p = p_front->out;	   				//ulozenie smernika na vyberany prvok fronty
	p_front->out = p_front->out->next;
	if (p_front->out == NULL) p_front->in = NULL;		//ak sme odstranili posledny prvok a front zostal prazdny,
	//vynulujeme ukazovatel na prvy prvok
	free((void *) pom_p);
	return pom;
}
//Vratenie, ci je front prazdny
int front_is_empty(front *p_front) {
		return (p_front->out == NULL);
}

