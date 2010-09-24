#include "trie.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static void die_usage(void) {
  fputs("usage: fuzzydups [-l limit] [-d limit] [-b base]\n", stderr);
  exit(1);
}

static void dups(struct trie *t, FILE *in, FILE *out, struct util_limit *ul)
{
  int first = 1;
  const char *line;

  while ((line = util_line(in))) {
    int limit = util_limit(ul, line);
    void *value = trie_find(t, line, limit);

    if (value) {
      if (!first)
        putc('\n', out);
      else
        first = 0;
      fprintf(out, "%s\n%s\n", line, (char *)value);
    }

    util_trie_insert(t, line);
  }
}

int main(int argc, char **argv)
{
  int opt;
  struct trie t;
  struct util_limit ul = UTIL_LIMIT_INIT;
  struct util_array bases = UTIL_ARRAY_INIT;
  unsigned i;

  while ((opt = getopt(argc, argv, "b:" UTIL_LIMIT_OPTS)) != -1) {
    if (opt == 'b') util_array_push(&bases, optarg);
    else if (!util_limit_opt(&ul, opt, optarg))
      die_usage();
  }
  argv += optind;
  argc -= optind;

  trie_init(&t);
  for (i = 0; i < bases.len; i++)
    util_trie_read(&t, util_open(bases.d[i], "r"));

  if (!argc)
    dups(&t, stdin, stdout, &ul);
  else
    for (i = 0; i < argc; i++)
      dups(&t, util_open(argv[i], "r"), stdout, &ul);
  return 0;
}
