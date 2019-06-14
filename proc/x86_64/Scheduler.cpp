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

Process *Scheduler::Reschedule() {
  if (curProc->proc.State == EXITED) {
    kfree(curProc);
    curProc = NULL;
  }

  // If there's nothing else to run, keep running the current process
  if (runQueue.Length() == 0) {
    if (curProc) {
      return &curProc->proc;
    }
    return NULL;
  }

  // If process is still running, re-add it to runnable queue
  if (curProc &&
      (curProc->proc.State == RUNNING || curProc->proc.State == RUNABLE)) {
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
