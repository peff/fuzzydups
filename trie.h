#ifndef TRIE_H
#define TRIE_H

struct trie_node {
  unsigned char terminal:1;
  void *value;
  struct trie_node *children[256];
};

struct trie {
  struct trie_node root;
};

void trie_init(struct trie *);
int trie_insert(struct trie *, const char *, void *value);
void *trie_find(const struct trie *, const char *, int limit);
void *trie_erase(struct trie *, const char *);

#endif /* TRIE_H */
