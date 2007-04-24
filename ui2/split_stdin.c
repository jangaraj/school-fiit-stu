#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **split_stdin(char *separators, int *num_fields, int *pocet) {
  int c, nfields, nchars, clen, flen;
  char *current;
  char **fields;
  current = (char *) calloc(10, sizeof(char));
  clen = 10;
  nchars = 0;
  fields = (char **) calloc(10, sizeof(char *));
  flen = 10;
  nfields = 0;
  *pocet = 0;
  do {
    c = getc(stdin);
    if (NULL == strchr(separators,c) && '\n' != c && EOF != c) {
      if (nchars == clen) {
		clen += clen;
		current = (char *) realloc(current, clen * sizeof(char));
      }
      current[nchars] = c;
      nchars++;
	  *pocet = *pocet + 1;
    }
    else if (nchars > 0) {
      current = (char *) realloc(current, (nchars+1) * sizeof(char));
      current[nchars] = '\0';
      if (nfields == flen) {
		flen += flen;
		fields = (char **) realloc(fields, flen * sizeof(char *));
      }
      fields[nfields] = current;
      nfields++;
      current = (char *) calloc(10, sizeof(char));
      clen = 10;
      nchars = 0;
    }
  }
  while ('\n' != c && EOF != c);
  free(current);
  fields = (char **) realloc(fields, nfields * sizeof(char *));
  if (EOF == c && nfields == 0) nfields = -1;
  *num_fields = nfields;
  return fields;
}

void free_split_stdin(char **response, int num_fields) {
  int i;
  for (i = 0; i < num_fields; i++) free(response[i]);
  free(response);
}
