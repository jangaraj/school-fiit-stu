#ifndef FILE_PRVOK_STROM_H_EXIST
#define FILE_PRVOK_STROM_H_EXIST

#include "stav.h"

typedef struct PRVOK_STROM {
	stav sta;
	char oper;	//predchadzajuci operator
	//nasledovnici vrchola
	struct PRVOK_STROM *child1;
	struct PRVOK_STROM *child2;
	struct PRVOK_STROM *child3;
	struct PRVOK_STROM *child4;
} prvok_strom;

#endif /* FILE_PRVOK_STROM_H_EXIST */
