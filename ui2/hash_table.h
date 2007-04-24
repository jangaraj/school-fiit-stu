#include "const.h"
#include "stav.h"

typedef struct {
     int *table;     //tabulka obsahuje hodnoty (0 alebo 1), podla ktorych je mozne urcit ci dany stav patri do mnoziny al    ebo nie
     long koef[POCET];
} HASH_TABLE;

HASH_TABLE *vytvor_tabulku(void);
int factorial(int i);
int hash(stav s, HASH_TABLE *p_hash_table);
void hash_insert(stav s, HASH_TABLE *p_hash_table);
int hash_is_in(stav s, HASH_TABLE *p_hash_table);
