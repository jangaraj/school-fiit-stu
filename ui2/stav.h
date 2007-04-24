#ifndef FILE_STAV_H_EXIST
#define FILE_STAV_H_EXIST

#include "const.h"

typedef struct {
	int polia[POCET];
} stav;

typedef struct STAV_S_HISTORIOU {
	stav s;
	char *operatory;
} stav_s_historiou;

#endif /* FILE_STAV_H_EXIST */
