#ifndef TABLE_H
#define TABLE_H

#include "hash_fn.h"

#define TABLE_TYPE unsigned long long
#define SPACE_FACTOR 6

/**
 * Main data structure for the hash table.
 * The table will store phone numbers,
 * represented as 13-digit unsigned integers.
 */


typedef struct bucket_node_t {
  TABLE_TYPE value;
  struct bucket_node_t *next;
} bucket_node;

typedef struct table_node_t {
  ulint count;
  bucket_node *bucket_head;
  hash_fn secondary_hash_function;
  TABLE_TYPE* secondary_table;
} table_node;

typedef struct table_t {
  ulint size;
  mpz_t universe_size;
  hash_fn primary_hash_function;
  table_node *primary_table;
} table;

void table_init(table *t, ulint size, mpz_t universe_size);

void table_insert(table *t, TABLE_TYPE v);

void table_build(table *t);

table * table_rehash(table *t);

int table_find(table *t, TABLE_TYPE v);

int table_node_insert(table_node *node, TABLE_TYPE v);

int table_node_find(table_node *node, TABLE_TYPE v);

void add_to_bucket(table_node *node, TABLE_TYPE v);

TABLE_TYPE pop_bucket(table_node *node);

void destroy_bucket(table_node *node);

void table_destroy(table *t);
#endif
