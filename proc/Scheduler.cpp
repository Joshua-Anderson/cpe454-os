#include <stddef.h>

#include "../Scheduler.h"
#include "kmalloc.h"

unsigned Scheduler::nxtPid = 0;
struct QueueEntry *Scheduler::curProc = NULL;
Queue Scheduler::runQueue = Queue();
Process Scheduler::ParentProc = Process(-1);

Process *Scheduler::Add(kentry_t entry, void *arg) {
  struct QueueEntry *p =
      (struct QueueEntry *)kmalloc(sizeof(struct QueueEntry));
  p->proc = Process(nxtPid, entry, arg);
  nxtPid++;
  p->nxt = p;
  p->prev = p;
  runQueue.Push(p);

  return &p->proc;
}

void Scheduler::Unblock(struct Queue *blockQueue) {
  for (unsigned i = 0; i < blockQueue->Length(); i++) {
    struct QueueEntry *entry = blockQueue->Pop();

    // Edge case where proc is blocked but unblocked before yielding
    if (entry == curProc) {
      entry->proc.State = RUNNING;
      continue;
    }

    entry->proc.State = RUNABLE;
    entry->block_data_1 = 0;
    entry->block_data_2 = 0;
    runQueue.Push(entry);
  }
}

Process *Scheduler::Reschedule() {
  if (curProc->proc.State == EXITED) {
    kfree(curProc);
    curProc = NULL;
  }

  // If there's nothing else to run, keep running the current process
  if (runQueue.Length() == 0) {
    if (curProc && curProc->proc.CanRun()) {
      return &curProc->proc;
    }
    return NULL;
  }

  // If process is still running, re-add it to runnable queue
  if (curProc && curProc->proc.CanRun()) {
    curProc->proc.State = RUNABLE;
    runQueue.Push(curProc);
  }

  curProc = runQueue.Pop();
  if (curProc == NULL) {
    return NULL;
  }

  curProc->proc.State = RUNNING;
  return &curProc->proc;
}

Process *Scheduler::GetCurProc() { return &curProc->proc; }

void Scheduler::BlockCurProc(Queue *blockQueue, uint64_t block_data_1,
                             uint64_t block_data_2) {
  curProc->proc.State = BLOCKED;
  curProc->block_data_1 = block_data_1;
  curProc->block_data_2 = block_data_2;
  blockQueue->Push(curProc);
}
