#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "heap.h"

tHEAP heap_init(unsigned size, int (*compare)(const void *e1, const void *e2)) {
  tHEAP r;

  r = (tHEAP) malloc(sizeof(struct heap));

  r->v = (void **) malloc(size * sizeof(void *));
  r->length = 0;
  r->size = size;
  r->compare = compare;

  return r;
}

void heap_destroy(tHEAP h) {
  free(h->v);
  free(h);
}

static void max_heapify(tHEAP h, unsigned i) {
  unsigned largest;
  const unsigned l = LEFT(i), r = RIGHT(i);

  if(l < h->length && (h->compare)(h->v[l], h->v[i]) > 0)
    largest = l;
  else
    largest = i;

  if(r < h->length && (h->compare)(h->v[r], h->v[largest]) > 0)
    largest = r;

  if(largest != i) {
    void *t = h->v[i];
    h->v[i] = h->v[largest];
    h->v[largest] = t;

    max_heapify(h, largest);
  }
}

static void build_max_heap(tHEAP h, unsigned n) {
  unsigned i;

  h->length = n;

  for(i = n/2; i >= 1; i--)
    max_heapify(h, i-1);
}

void heapsort(tHEAP h, unsigned n) {
  unsigned i;

  build_max_heap(h, n);

  for(i=n-1; i>=1; i--) {
    void *t = h->v[0];
    h->v[0] = h->v[i];
    h->v[i] = t;

    h->length--;
    max_heapify(h, 0);
  }
}

void *heap_extract_max(tHEAP h) {
  void *max;

  if(h->length < 1) /* underflow */
    return NULL;

  max = h->v[0];
  h->v[0] = h->v[h->length-1];
  h->length--;
  max_heapify(h, 0);

  return max;
}

static void heap_increase_key(tHEAP h, unsigned i, void *key) {
  if((h->compare)(key, h->v[i]) < 0) /* key is smaller than current key */
    return;

  h->v[i] = key;

  while(i > 0 && (h->compare)(h->v[PARENT(i)], h->v[i]) < 0) {
    void *t = h->v[i];
    h->v[i] = h->v[PARENT(i)];
    h->v[PARENT(i)] = t;
    i = PARENT(i);
  }
}

void max_heap_insert(tHEAP h, void *key) {

  if(h->length == h->size) {
    if((h->v = (void **) realloc(h->v, 2 * h->size * sizeof(void *))) == NULL) {
      perror("realloc");
      exit(-1);
    }
    h->size *= 2;
  }

  h->length++;
  h->v[h->length-1] = NULL;
  heap_increase_key(h, h->length-1, key);
}
