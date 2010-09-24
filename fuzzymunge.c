#include "trie.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void die_usage(void) {
  die("%s",
"usage: fuzzymunge [-v] [-l limit] [-d limit]\n"
"                  [-b base] [-i input] [-o output]\n"
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
  struct util_array inputs = UTIL_ARRAY_INIT;
  struct util_array outputs = UTIL_ARRAY_INIT;

  while ((opt = getopt(argc, argv, "b:i:o:" UTIL_LIMIT_OPTS)) != -1) {
    if (opt == 'b') util_array_push(&bases, optarg);
    else if(opt == 'i') util_array_push(&inputs, optarg);
    else if(opt == 'o') util_array_push(&outputs, optarg);
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

  if (!inputs.len) {
    if (outputs.len > 1)
      die("can only use one output when reading from stdin");
    munge(&t, stdin, outputs.len ? util_open(outputs.d[0], "w") : stdout, &ul);
  }
  else {
    unsigned i;
    if (outputs.len && outputs.len != inputs.len)
      die("number of inputs and outputs must match");
    for (i = 0; i < inputs.len; i++)
      munge(&t, util_open(inputs.d[i], "r"),
                outputs.len ? util_open(outputs.d[i], "w") : stdout, &ul);
  }

  return 0;
}
