#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 255

int main(void) 
{
	char vstup[MAX_LINE_LENGTH];
//	char *out;
//	int i;

//	i = 1;
    /*if((out = (char *) malloc(sizeof(char)))==NULL) {
         fprintf(stderr,"Error, malloc pole_start\n");
         exit (1);
    }*/
//	*out = '\0';
	while(fgets(vstup,MAX_LINE_LENGTH,stdin)!=NULL) {
//		out = (char *) realloc(out,i+1 * sizeof(char));
//		i++;
		if(vstup[0]!='-') {
//			strcat(out,vstup+sizeof(char));
//			strcat(out,"\n");
			printf("-%d\n",atol(vstup));
		}
		else {
//			strcat(out,vstup);
//			strcat(out,"\n");
			printf("%d\n",atol(vstup));
		}
	}
//	printf("%s",out);
	return 0;
}
