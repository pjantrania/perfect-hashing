#ifndef HASH_FN_H
#define HASH_FN_H
#define STRING_ALPHABET_SIZE 26
#define SEED 3234213
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

typedef unsigned long int ulint;

typedef struct hash_fn_t {
  mpz_t p;
  mpz_t a;
  mpz_t b;
  mpz_t m;
  gmp_randstate_t state;
  
} hash_fn;


/**
 * Initialize the parameters for the function.
 *
 */
void hash_fn_init(hash_fn *f, ulint table_size);

/**
 * Generate values for p, a, and b.
 *
 */
void hash_fn_generate(hash_fn *f, mpz_t u);

/**
 * Apply hash function to a string.
 *
 */
void hash_string(const hash_fn *f, mpz_t out, const char *string);

/**
 * Apply hash function to an integer.
 *
 */
void hash_integer(const hash_fn *f, mpz_t out, mpz_t integer);

/**
 * Deallocate the function's parameters.
 *
 */
void hash_fn_destroy(hash_fn *f);

#endif
