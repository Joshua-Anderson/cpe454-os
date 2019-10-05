#include <stddef.h>

#include "kmalloc.h"
#include "mm/Page.h"
#include "printk.h"
#include "stdlib.h"

struct FreeEntry {
  struct FreeEntry *prev;
} __attribute__((packed));

struct KmallocPool {
  unsigned size;
  struct FreeEntry *free_entries;
};

struct KmallocEntry {
  uint32_t size;
  struct KmallocPool *pool;
} __attribute__((packed));

#define NUM_POOLS 5
static struct KmallocPool kmalloc_pools[NUM_POOLS] = {
    {.size = 32, .free_entries = NULL},   {.size = 128, .free_entries = NULL},
    {.size = 512, .free_entries = NULL},  {.size = 1024, .free_entries = NULL},
    {.size = 2048, .free_entries = NULL},
};

#define REMOVE_KMALLOC_ENTRY(x) \
  ((void *)((intptr_t)x + sizeof(struct KmallocEntry)))
#define GET_KMALLOC_ENTRY(x) \
  ((struct KmallocEntry *)((intptr_t)x - sizeof(struct KmallocEntry)))

void *kmalloc(uint32_t size) {
  for (int i = 0; i < NUM_POOLS; i++) {
    struct KmallocPool *p = &kmalloc_pools[i];
    if (p->size < size + sizeof(struct KmallocEntry)) {
      continue;
    }

    if (!p->free_entries) {
      void *page = Page::AllocKernHeapPage();
      for (unsigned j = 0; j < Page::PAGE_SIZE / p->size; j++) {
        struct FreeEntry *entry =
            (struct FreeEntry *)((intptr_t)page + p->size * j);
        entry->prev = p->free_entries;
        p->free_entries = entry;
      }
    }

    struct KmallocEntry *entry = (struct KmallocEntry *)p->free_entries;
    p->free_entries = p->free_entries->prev;
    entry->size = size;
    entry->pool = p;

    return REMOVE_KMALLOC_ENTRY(entry);
  }

  // If the malloc request can't fit within a pool, allocate space just for it.
  uint32_t alloced =
      ROUND_UP(size + sizeof(struct KmallocEntry), Page::PAGE_SIZE);
  struct KmallocEntry *entry =
      (struct KmallocEntry *)Page::AllocKernHeap(alloced);
  entry->size = size;
  entry->pool = NULL;
  return REMOVE_KMALLOC_ENTRY(entry);
}

void kfree(void *ptr) {
  struct KmallocEntry *entry = GET_KMALLOC_ENTRY(ptr);

  if (entry->pool) {
    struct KmallocPool *p = entry->pool;
    struct FreeEntry *fentry = (struct FreeEntry *)entry;
    fentry->prev = p->free_entries;
    p->free_entries = fentry;
    return;
  }

  uint32_t alloced =
      ROUND_UP(entry->size + sizeof(struct KmallocEntry), Page::PAGE_SIZE);
  Page::FreeKernHeap((void *)entry, alloced);
}
