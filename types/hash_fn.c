#include <time.h>
#include <sys/time.h>
#include "hash_fn.h"

void hash_fn_init(hash_fn *f, ulint table_size) {
  mpz_init(f->a);
  mpz_init(f->b);
  mpz_init(f->p);
  mpz_init(f->m);
  gmp_randinit_default(f->state);

  mpz_set_ui(f->m, table_size);
}

void hash_fn_generate(hash_fn *f, mpz_t u) {
  struct timeval time;
  ulint microtime;

  gettimeofday(&time, NULL);
  microtime = (ulint)time.tv_sec*1000000 + (ulint)time.tv_usec;
  
  gmp_randseed_ui(f->state, microtime);
  mpz_nextprime(f->p, u);
  mpz_urandomm(f->a, f->state, f->p);
  mpz_urandomm(f->b, f->state, f->p);

}

void hash_string(const hash_fn *f, mpz_t out, const char *string) {
  hash_fn inner_hash;
  mpz_t u, xi, ai;
  size_t l;
  unsigned int i;

  mpz_init(u);
  mpz_init(xi);
  mpz_init(ai);
  mpz_set_ui(u, STRING_ALPHABET_SIZE);
  mpz_set_ui(out, 0);
  hash_fn_init(&inner_hash, STRING_ALPHABET_SIZE);
  
  hash_fn_generate(&inner_hash, f->m);

  l = strlen(string);
  for ( i = 0; i < l; i++ ) {
    ulint x = ( string[i] <= 'z' ? 
		string[i] - 'a' : 
		string[i] - 'A' );
    mpz_set_ui(xi, x);
    mpz_powm_ui(ai, inner_hash.a, (ulint)i, inner_hash.p);
    mpz_addmul(out, ai, xi);
  }

  mpz_mod(out, out, inner_hash.p);
  hash_integer(f, out, out);

}

void hash_integer(const hash_fn *f, mpz_t out, mpz_t integer) {
  mpz_set_ui(out, 0);
  mpz_mul(out, f->a, integer);
  mpz_add(out, out, f->b);
  mpz_mod(out, out, f->p);
  mpz_mod(out, out, f->m);
}

void hash_fn_destroy(hash_fn *f) {
  mpz_clear(f->a);
  mpz_clear(f->b);
  mpz_clear(f->p);
  mpz_clear(f->m);
  gmp_randclear(f->state);

}
