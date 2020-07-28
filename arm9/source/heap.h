#ifndef HEAP_H
#define HEAP_H

#define LEFT(i) (2*(i+1)-1)
#define RIGHT(i) (2*(i+1))
#define PARENT(i) ((i+1)/2-1)

struct heap {
  void **v;

  unsigned length;
  unsigned size;

  int (*compare)(const void *e1, const void *e2);
};

typedef struct heap *tHEAP;

tHEAP heap_init(unsigned size, int (*compare)(const void *e1, const void *e2));

void heap_destroy(tHEAP h);

void *heap_extract_max(tHEAP h);

void max_heap_insert(tHEAP h, void *key);

#endif
