#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include "trie.h"

void die(const char *fmt, ...);

FILE *util_open(const char *fn, const char *mode);
const char *util_line(FILE *in);

void util_trie_read(struct trie *t, FILE *in);
void util_trie_insert(struct trie *t, const char *s);

struct util_limit {
  int dynamic;
  int limit;
  int max;
};
#define UTIL_LIMIT_INIT { 1, 5, 3 };
#define UTIL_LIMIT_OPTS "l:d:m:"
int util_limit_opt(struct util_limit *, char opt, const char *arg);
int util_limit(struct util_limit *, const char *s);

struct util_array {
  const char **d;
  unsigned len;
  unsigned alloc;
};
#define UTIL_ARRAY_INIT { NULL, 0, 0 }
void util_array_push(struct util_array *, const char *);

#endif /* UTIL_H */
