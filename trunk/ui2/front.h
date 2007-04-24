#ifndef FILE_FRONT_H_EXIST
#define FILE_FRONT_H_EXIST

#include "prvok_strom.h"
#include "stav.h"

typedef struct PRVOK_FRONT {
	void *c;
	struct PRVOK_FRONT *next;
} prvok_front;

typedef struct FRONT {
	prvok_front *in;		//prvy prvok frontu
	prvok_front *out;		//posledny prvok frontu
} front;

void front_insert(prvok_strom *c, front *p_front);
void front_insert2(stav_s_historiou *c, front *p_front);
prvok_strom *front_get(front *p_front);
stav_s_historiou *front_get2(front *p_front);
int front_is_empty(front *p_front);

#endif /* FILE_FRONT_H_EXIST */
