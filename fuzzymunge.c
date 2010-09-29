#include "trie.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void die_usage(void) {
  die("%s",
"usage: fuzzymunge [-v] [-l limit] [-d limit]\n"
"                  [-b base] [-i] [<files...>]\n"
);
}

static void munge(const struct trie *t, FILE *in, FILE *out,
                  struct util_limit *ul)
{
  const char *line;

  while ((line = util_line(in))) {
    int limit = util_limit(ul, line);
    void *value = trie_find(t, line, limit);
    if (value)
      fputs(value, out);
    else {
      fprintf(stderr, "warning: no match for '%s'\n", line);
      fputs(line, out);
    }
    putc('\n', out);
  }
}

int main(int argc, char **argv)
{
  int opt;
  struct trie t;
  struct util_limit ul = UTIL_LIMIT_INIT;
  struct util_array bases = UTIL_ARRAY_INIT;
  int inplace = 0;

  while ((opt = getopt(argc, argv, "b:i" UTIL_LIMIT_OPTS)) != -1) {
    if (opt == 'b') util_array_push(&bases, optarg);
    else if(opt == 'i') inplace = 1;
    else if (!util_limit_opt(&ul, opt, optarg))
      die_usage();
  }
  argv += optind;
  argc -= optind;

  trie_init(&t);
  if (!bases.len)
    util_trie_read(&t, stdin);
  else {
    unsigned i;
    for (i = 0; i < bases.len; i++)
      util_trie_read(&t, util_open(bases.d[i], "r"));
  }

  if (!*argv)
    munge(&t, stdin, stdout, &ul);

  for (; *argv; argv++) {
    FILE *in = util_open(*argv, "r");

    if (inplace) {
      struct util_atomicfile tmp;
      util_atomicfile_open(&tmp, *argv);
      munge(&t, in, tmp.fh, &ul);
      util_atomicfile_close(&tmp, *argv);
    }
    else
      munge(&t, in, stdout, &ul);
  }

  return 0;
}
