typedef struct GEN {
	char *genik;
	struct GEN *next;
	struct GEN *prev;
} gen;

typedef struct GENOTYP {
	gen *geny;
	char *funkcia;
	double fitnes;
	int dlzka;
	struct GENOTYP *next;
} genotyp;

gen *random_geny(char *pole_genov[], genotyp *jedinec);
char *urob_funkciu(genotyp *jedinec, int recheck); 
double evalvuj(char *fcia, int x);
genotyp *turnaj(genotyp *in, char *pole_genov[]);
gen *mutacia(genotyp *in, char *pole_genov[]);
genotyp *zotried(genotyp *in);
genotyp *ruleta(genotyp *in, double sum);
genotyp *orezanie(genotyp *in, char *pole_genov[]);
genotyp *krizenie(genotyp *in, int pocet);
