#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "Process.h"

extern "C" struct QueueEntry;

class Queue {
 public:
  void Push(struct QueueEntry*);
  struct QueueEntry* Pop();
  struct QueueEntry* Peak();
  unsigned Length();

 private:
  struct QueueEntry* head = NULL;
  struct QueueEntry* tail = NULL;
  unsigned len = 0;
};

struct QueueEntry {
  Process proc;
  Queue* queue;
  struct QueueEntry* nxt;
  struct QueueEntry* prev;
};

#endif
