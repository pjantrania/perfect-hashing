#include "types/table.h"
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char **argv) {
  if ( argc != 3 ) {
    printf("Usage: %s (filename) (number of entries)\n", argv[0]);
    exit(1);
  }

  FILE *fp;
  char buf[12];
  table t;
  mpz_t u;
  ulint i, count, inserted, found;
  unsigned long long phonenum;
  unsigned long long *numlist;

  if ( (fp = fopen(argv[1], "r")) == NULL ) {
    printf("Could not open file: %s\n", argv[1]);
  }

  sscanf(argv[2], "%ld", &count);
  numlist = (unsigned long long *)calloc( count, sizeof(unsigned long long));
  
  mpz_init(u);
  mpz_set_ui(u, 1000 * 1000);
  mpz_mul_ui(u, u, 1000);
  table_init(&t, count, u);

  i = 0;
  while ( fgets(buf, 12, fp) != NULL ) {
    sscanf(buf, "%lld\n", &phonenum);
    numlist[i] = phonenum;
    table_insert(&t, phonenum);
    i++;
  }
  fclose(fp);
  table_build(&t);
  printf("Done inserting %ld numbers\n", i);
  
  i = 0;
  found = 0;
  while ( i < count ) {
    if ( table_find(&t, numlist[i]) ) {
      found++;
    }
    i++;
  }

  printf("Found %ld numbers\n", found);

  free(numlist);
  table_destroy(&t);
  mpz_clear(u);

}
