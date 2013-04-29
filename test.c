#include "types/table.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define PROB_INVALID .05

void main(int argc, char **argv) {
  if ( argc != 3 ) {
    printf("Usage: %s (filename) (number of entries)\n", argv[0]);
    exit(1);
  }

  FILE *fp;
  char buf[12];
  table t;
  mpz_t u;
  ulint i, count, invalid, found;
  unsigned long long phonenum;
  unsigned long long *numlist;
  struct timeval starttime, endtime;
  ulint starttime_us, endtime_us;

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
  invalid = 0;
  while ( fgets(buf, 12, fp) != NULL ) {
    sscanf(buf, "%lld\n", &phonenum);
    numlist[i] = phonenum;
    if (rand() <= PROB_INVALID) {
      numlist[i] = numlist[i] / 4;
      invalid++;
    }
    table_insert(&t, phonenum);
    i++;
  }
  fclose(fp);
  table_build(&t);
  printf("Done inserting %ld numbers (%ld invalids)\n", i, invalid);
  
  i = 0;
  found = 0;
  invalid = 0;
  gettimeofday(&starttime, NULL);
  while ( i < count ) {
    if ( table_find(&t, numlist[i]) ) {
      found++;
    } else {
      invalid++;
    }
    i++;
  }
  gettimeofday(&endtime, NULL);
  starttime_us = starttime.tv_sec*1000000 + starttime.tv_usec;
  endtime_us = endtime.tv_sec*1000000 + endtime.tv_usec;

  printf("Found %ld numbers, rejected %ld\n", found, invalid);
  printf("Total find time: %ld us\n", endtime_us-starttime_us);

  free(numlist);
  table_destroy(&t);
  mpz_clear(u);

}
