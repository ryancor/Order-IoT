#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE       80
#define M2C           (256 * 256)
#define M3C           (256 * 256 * 256)
#define fp_t          double

struct stats {
  fp_t *m2, *m3;
  char *arch;
};

int verbose = 0;

void increment(struct stats r, unsigned char *buffer, size_t c) {
  size_t i;

  for(i = 2; i < c; i++) {
    r.m2[buffer[i] + 256 * buffer[i-1]] += 1;
    r.m3[buffer[i] + 256 * buffer[i-1] + 256*256*buffer[i-2]] += 1;
  }
}

struct stats count_ngrams(char *filename) {
  // allocate 128mbs
  FILE *f;
  struct stats r;
  unsigned char buffer[BUFSIZE];
  unsigned char prv, prv2;
  size_t c;

  f = fopen(filename, "rb");
  if(f == NULL) {
    perror(filename);
    exit(EXIT_FAILURE);
  }

  r.m2 = static_cast<double*>(malloc(M2C * sizeof(fp_t)));
  if(r.m2 == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }

  r.m3 = static_cast<double*>(malloc(M3C * sizeof(fp_t)));
  if(r.m3 == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }

  c = fread(buffer, 1, BUFSIZE, f);
  if(c > 1) {
    r.m2[buffer[1] + 256 * buffer[0]] += 1;
  }
  increment(r, buffer, c);

  while(c) {
    buffer[0] = buffer[BUFSIZE-2];
    buffer[1] = buffer[BUFSIZE-1];
    c = fread(buffer+2, 1, BUFSIZE-1, f);
    increment(r, buffer, c+2);
  }

  fclose(f);
  return r;
}

void make_frequencies(struct stats s, fp_t base) {
  size_t i;
  fp_t f;

  f = 0;
  for(i = 0; i < M2C; i++) {
    s.m2[i] += base;
    f += s.m2[i];
  }

  for(i = 0; i < M2C; i++) {
    s.m2[i] /= f;
  }
  f = 0;

  for(i = 0; i < M3C; i++) {
    s.m3[i] += base;
    f += s.m3[i];
  }

  for(i = 0; i < M3C; i++) {
    s.m3[i] /= f;
  }
}

#define BASEDIR "/tmp/cpu_rec_corpus"
#define MAX_ARCH 1000

void create_dir_if_not() {
    struct stat st = {0};

    if(stat(BASEDIR, &st) == -1) {
      mkdir(BASEDIR, 0700);
    }

    int empty_f = open("/tmp/cpu_rec_corpus/test.corpus",
      O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    if(empty_f == -1) {
      printf("Error creating file in %s\n", BASEDIR);
    }
}

struct stats *read_corpus() {
  size_t h, i;
  struct stats *s;
  DIR *d;

  create_dir_if_not();

  d = opendir(BASEDIR);
  if(d == NULL) {
    perror(BASEDIR);
    exit(EXIT_FAILURE);
  }

  s = static_cast<struct stats*>(malloc(MAX_ARCH * sizeof(struct stats)));
  h = 0;
  while(1) {
    char filename[100];
    struct dirent *f = readdir(d);

    if(f == NULL) {
      break;
    }

    if((f->d_namlen < 7) || strncmp(f->d_name + f->d_namlen-7, ".corpus", 7)) {
      continue;
    }

    snprintf(filename, 100, BASEDIR "/%s", filename);
    if(verbose) {
      printf("* %s\n", filename);
    }
    s[h] = count_ngrams(filename);
    s[h].arch = strndup(f->d_name, f->d_namlen-7);

    make_frequencies(s[h], 0.01);
    h++;
    if(h >= MAX_ARCH) {
      break;
    }
  }

  return s;
}

fp_t KLdivergence(fp_t *P, fp_t *Q, size_t sz) {
  size_t i;
  fp_t k = 0;

  for(i = 0; i < sz; i++) {
    if(P[i]) {
      k += P[i] * log(P[i] / Q[i]);
    }
  }
  return k;
}

void list_cpu_rec_after_check(char *file) {
  size_t h, i;
  struct stats *corpus = read_corpus();
  verbose = 1;

  struct stats c = count_ngrams(file);
  make_frequencies(c, 0);

  for(h = 0; h < MAX_ARCH; h++) {
    if(!corpus[h].arch) {
      break;
    }

    fp_t k2 = KLdivergence(c.m2, corpus[h].m2, M2C);
    fp_t k3 = KLdivergence(c.m3, corpus[h].m3, M3C);
    if(verbose) {
      printf("Arch: %10s\tsizes: %f :: %f\n", corpus[h].arch, k2, k3);
    }
  }
}
