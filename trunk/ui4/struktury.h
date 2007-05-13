typedef struct fakty
{
	char fakt[40];
	fakty *next;
}
fakty;

typedef struct def_pravidlo
{
	char meno[13];
	char podmienka[80];
	char akcia[80];
	def_pravidlo *next;
}def_pravidlo;
