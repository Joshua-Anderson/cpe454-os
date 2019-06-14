#include "Queue.h"
#include "printk.h"

void Queue::Push(struct QueueEntry* entry) {
  if (entry->queue != NULL) {
    FATAL("Tried to enqueue already queued process");
  }

  Queue::len++;
  entry->queue = this;
  entry->nxt = NULL;

  if (head == NULL) {
    head = tail = entry;
    head->prev = NULL;
  }

  tail->nxt = entry;
  entry->prev = tail;
  tail = entry;
}

struct QueueEntry* Queue::Pop() {
  if (len == 0) {
    return NULL;
  }

  len--;
  struct QueueEntry* entry = head;
  head = head->nxt;
  if (head) {
    head->prev = NULL;
  }

  entry->queue = NULL;
  entry->nxt = NULL;
  return entry;
}

struct QueueEntry* Queue::Peak() {
  return head;
}

unsigned Queue::Length() { return len; }
