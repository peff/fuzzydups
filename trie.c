#include "trie.h"
#include <stdlib.h>
#include <string.h>

struct trie_find_result {
  int limit;
  const struct trie_node *node;
};

static void trie_node_init(struct trie_node *tn)
{
  memset(tn, 0, sizeof(*tn));
}

static void trie_find_recurse(const struct trie_node *cur,
                              const unsigned char *k,
                              int limit, struct trie_find_result *r)
{
  unsigned i;

  /* If we are out of key and at a terminal node,
   * we have a match. Is it better than the current best? */
  if (!*k && cur->terminal) {
    if (limit > r->limit) {
      r->limit = limit;
      r->node = cur;
    }
    return;
  }

  /* Try an exact match of this element. */
  if (*k && cur->children[*k])
    trie_find_recurse(cur->children[*k], k+1, limit, r);

  /* If we have no limit to spare, or if we are already as bad as than
   * something we have already found, there's no point in continuing. */
  if (!limit || limit <= r->limit)
    return;

  /* Try deleting this element. */
  if (*k)
    trie_find_recurse(cur, k+1, limit-1, r);

  /* Try substitution or addition. */
  for (i = 0; i < 256; i++) {
    if (!cur->children[i])
      continue;
    /* Substitution. Avoid substituting for ourselves,
     * as we will already have tried that with exact match above. */
    if (*k && i != *k)
      trie_find_recurse(cur->children[i], k+1, limit-1, r);
    /* Addition. */
    trie_find_recurse(cur->children[i], k, limit-1, r);
  }
}

void trie_init(struct trie *t)
{
  trie_node_init(&t->root);
}

int trie_insert(struct trie *t, const char *ks, void *value)
{
  struct trie_node *cur = &t->root;
  const unsigned char *k = (const unsigned char *)ks;
  for (; *k; k++) {
    if (!cur->children[*k]) {
      cur->children[*k] = malloc(sizeof(*cur->children[*k]));
      if (!cur->children[*k]) return -1;
      trie_node_init(cur->children[*k]);
    }
    cur = cur->children[*k];
  }
  cur->terminal = 1;
  cur->value = value;
  return 0;
}

static int trie_erase_recurse(struct trie_node *cur, const unsigned char *k,
                              void **value)
{
  unsigned i;

  /* If we're at the end... */
  if (!*k) {
    /* And we're a terminal, then we are actually erasing something */
    if (cur->terminal) {
      *value = cur->value;
      cur->terminal = 0;
    }
    /* Report back whether we can be erased */
    for (i = 0; i < 256; i++)
      if (cur->children[i])
        return 0;
    return 1;
  }

  /* Otherwise, if we can continue, do so. */
  if (cur->children[*k]) {
    /* If our child can't be erased, neither can we. */
    if (!trie_erase_recurse(cur->children[*k], k+1, value))
      return 0;
    /* Otherwise, erase the child... */
    free(cur->children[*k]);
    cur->children[*k] = NULL;
    /* ... and fall through to the missing case */
  }
  /* see if we can be erased */
  if (cur->terminal) return 0;
  for (i = 0; i < 256; i++)
    if (cur->children[i])
      return 0;
  return 1;
}

void *trie_erase(struct trie *t, const char *k)
{
  void *value = NULL;
  /* ignore return value; we never erase the root node */
  trie_erase_recurse(&t->root, (const unsigned char *)k, &value);
  return value;
}

void *trie_find(const struct trie *t, const char *k, int limit)
{
  struct trie_find_result r = { -1, NULL };
  trie_find_recurse(&t->root, (const unsigned char *)k, limit, &r);
  return r.node ? r.node->value : NULL;
}
