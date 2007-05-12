typedef struct fakty
{
	char fakt[40];
	fakty *next;
}
fakty;

typedef struct ppravidla
{
	char meno[13];
	char podmienka[80];
	char akcia[80];
	ppravidla *next;
}ppravidla;
