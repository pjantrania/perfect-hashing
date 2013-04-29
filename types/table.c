#include <stdlib.h>
#include <gmp.h>
#include "table.h"
#include "hash_fn.h"

void table_init(table *t, ulint size, mpz_t universe_size) {
  t->size = size;
  mpz_init(t->universe_size);
  mpz_set(t->universe_size, universe_size);

  hash_fn_init(&t->primary_hash_function, size);
  hash_fn_generate(&t->primary_hash_function, universe_size);
  t->primary_table = (table_node *) calloc( size, sizeof(table_node) );
}

void table_build(table *t) {
  ulint i, j, r, bucket_size, count_iter;
  mpz_t total_size, node_size;
  table_node *n;
  bucket_node *iter;
  
  // Check whether our table violates the space constraint
  mpz_init(total_size);
  mpz_set_ui(total_size, 0);

  count_iter = 0;
  for ( ;; ) {
    for ( i = 0; i < t->size; i++ ) {
      bucket_size = t->primary_table[i].count;
      bucket_size *= bucket_size;
      mpz_add_ui(total_size, total_size, bucket_size);
    }
    count_iter++;
    if ( mpz_cmp_ui(total_size, (t->size * SPACE_FACTOR)) > 0 ) {
      if ( count_iter > 20 ) {
	printf("Too many iterations of size control loop\n");
	exit(1);
      }
      t = table_rehash(t);
    } else
      break;
    
  }
  
  mpz_clear(total_size);
  mpz_init(node_size);

  count_iter = 0;
  for ( i = 0; i < t->size; i++ ) {
    if ( t->primary_table[i].count > 0 ) {
      n = &t->primary_table[i];
      mpz_set_ui(node_size, n->count * n->count);
      mpz_nextprime(node_size, node_size);
      n->secondary_table = (TABLE_TYPE*) calloc(mpz_get_ui(node_size), sizeof(TABLE_TYPE));
      hash_fn_init(&n->secondary_hash_function, n->count * n->count);
      hash_fn_generate(&n->secondary_hash_function, t->universe_size);

      j = 0;
      iter = n->bucket_head;
      while (  j < n->count ) {
	r = table_node_insert(n, iter->value);
	if ( r == 1 ) {
	  count_iter++;
	  if ( count_iter > 7500 ) {
	    printf("Having trouble inserting %lld into node %ld\n", iter->value, i);
	    printf("Function: %ld x + %ld mod %ld mod %ld\n", mpz_get_ui(n->secondary_hash_function.a),
		   mpz_get_ui(n->secondary_hash_function.b), mpz_get_ui(n->secondary_hash_function.p),
		   mpz_get_ui(n->secondary_hash_function.m));
	    exit(1);
	  }
	  hash_fn_generate(&n->secondary_hash_function, t->universe_size);
	  iter = n->bucket_head;
	  j = 0;
	  continue;
	} else if ( j < n->count - 1 ) {
	  iter = iter->next;
	}
	j++;
	
      }

      for ( j = 0; j < n->count; j++ )
	pop_bucket(n);
    }
  }

  mpz_clear(node_size);

}

table * table_rehash(table *t) {
  table *new_table;
  ulint i, j;

  new_table = (table *) calloc( 1, sizeof(table) );
  table_init(new_table, t->size, t->universe_size);

  for ( i = 0; i < t->size; i++ ) {
    if ( t->primary_table[i].count > 0 ) {
      for ( j = 0; j < t->primary_table[i].count; j++ ) {
	table_insert(new_table, pop_bucket(&t->primary_table[i]));
      }
      t->primary_table[i].count = 0;
    }
  }
  
  table_destroy(t);
  free(t);
  return new_table;
}

void table_insert(table *t, TABLE_TYPE v) {
  mpz_t value, position;
  ulint p;
  
  mpz_init(value);
  mpz_init(position);
  mpz_set_ui(value, v);
  hash_integer((const hash_fn *) &t->primary_hash_function, position, value);
  add_to_bucket(&t->primary_table[mpz_get_ui(position)], v);
  
  mpz_clear(value);
  mpz_clear(position);
}

int table_find(table *t, TABLE_TYPE v) {
  mpz_t value, position;
  ulint p;
  
  mpz_init(value);
  mpz_init(position);
  mpz_set_ui(value, v);
  hash_integer((const hash_fn *) &t->primary_hash_function, position, value);
  p = mpz_get_ui(position);
  mpz_clear(value);
  mpz_clear(position);
  
  return (t->primary_table[p].count == 0 ? 0 : table_node_find(&t->primary_table[p], v));
}

void add_to_bucket(table_node *node, TABLE_TYPE v) {
  bucket_node *item;
  
  item = (bucket_node *) malloc( sizeof(bucket_node) );
  item->value = v;
  item->next = node->bucket_head;
  node->bucket_head = item;
  node->count++;
}

TABLE_TYPE pop_bucket(table_node *node) {
  TABLE_TYPE value;
  bucket_node *temp;
  
  value = node->bucket_head->value;
  temp = node->bucket_head;
  node->bucket_head = node->bucket_head->next;
  free(temp);

  return value;
}

int table_node_insert(table_node *node, TABLE_TYPE v) {
  mpz_t value, position;
  ulint p;
  
  mpz_init(value);
  mpz_init(position);
  mpz_set_ui(value, v);
  hash_integer((const hash_fn *) &node->secondary_hash_function, position, value);
  p = mpz_get_ui(position);
  mpz_clear(value);
  mpz_clear(position);
  
  if ( node->secondary_table[p] != 0 ) {
    return 1;
  }
  node->secondary_table[p] = v;
  return 0;
}

int table_node_find(table_node *node, TABLE_TYPE v) {
  mpz_t value, position;
  ulint p;
  
  mpz_init(value);
  mpz_init(position);
  mpz_set_ui(value, v);
  hash_integer((const hash_fn *) &node->secondary_hash_function, position, value);
  p = mpz_get_ui(position);
  mpz_clear(value);
  mpz_clear(position);
  
  return (node->secondary_table[p] == v ? 1 : 0);

}

void table_destroy(table *t) {
  ulint i;
  
  hash_fn_destroy(&t->primary_hash_function);
  mpz_clear(t->universe_size);
  for ( i = 0; i < t->size; i++ ) {
    if ( t->primary_table[i].bucket_head != NULL )
      destroy_bucket(&t->primary_table[i]);
    else if ( t->primary_table[i].count > 0 ) {
      hash_fn_destroy(&t->primary_table[i].secondary_hash_function);
      free(t->primary_table[i].secondary_table);
    }
  }
  free(t->primary_table);
}

void destroy_bucket(table_node *node) {
  ulint i; 
  for ( i = 0; i < node->count; i++ ) {
    pop_bucket(node);
  }
  hash_fn_destroy(&node->secondary_hash_function);
  free(node->secondary_table);
  
}
