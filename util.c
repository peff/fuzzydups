#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

void die(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  putc('\n', stderr);
  exit(1);
}

static void die_errno(const char *fmt, ...)
{
  int saved_errno = errno;
  char buf[1024];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  die("%s: %s", buf, strerror(saved_errno));
}

static void die_nomem(void)
{
  die("out of memory");
}

static void chomp(char *s)
{
  int len = strlen(s) - 1;
  while (s[len] == '\n')
    s[len--] = '\0';
}

const char *util_line(FILE *in)
{
  static char buf[4096];
  if (!fgets(buf, sizeof(buf), in))
    return NULL;
  chomp(buf);
  return buf;
}

void util_trie_insert(struct trie *t, const char *s)
{
  char *v = strdup(s);
  if (!v) die_nomem();
  if (trie_insert(t, v, v) < 0) die_nomem();
}

FILE *util_open(const char *fn, const char *mode) {
  FILE *fh =  fopen(fn, mode);
  if (!fh)
    die_errno("unable to open %s", fn);
  return fh;
}

int util_limit_opt(struct util_limit *ul, char opt, const char *arg)
{
  switch (opt) {
    case 'l':
      ul->limit = atoi(arg);
      ul->dynamic = 0;
      return 1;
    case 'd':
      ul->limit = atoi(arg);
      ul->dynamic = 1;
      return 1;
    case 'm':
      ul->max = atoi(arg);
      return 1;
  }
  return 0;
}

int util_limit(struct util_limit *ul, const char *s)
{
  int limit = ul->dynamic ?
                ((strlen(s) - 1 + ul->limit) / ul->limit) :
                ul->limit;
  if (limit > ul->max)
    limit = ul->max;
  return limit;
}

void util_array_push(struct util_array *ua, const char *v)
{
  if (ua->len == ua->alloc) {
    ua->alloc = (ua->alloc+16)*3/2;
    ua->d = realloc(ua->d, ua->alloc * sizeof(*ua->d));
    if (!ua->d) die_nomem();
  }
  ua->d[ua->len++] = v;
}

void util_trie_read(struct trie *t, FILE *in)
{
  const char *line;
  while ((line = util_line(in))) {
    if (!*line) break;
    util_trie_insert(t, line);
  }
}
