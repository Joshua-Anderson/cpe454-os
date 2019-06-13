#include <stddef.h>

#include "../Scheduler.h"
#include "kmalloc.h"

struct ProcQueueEntry *Scheduler::cur_proc = NULL;
Process Scheduler::parent_proc = Process();

void Scheduler::Add(kentry_t entry, void *arg) {
  struct ProcQueueEntry *p =
      (struct ProcQueueEntry *)kmalloc(sizeof(struct ProcQueueEntry));
  p->proc = Process(entry, arg);
  p->nxt = p;
  p->prev = p;

  if (!Scheduler::cur_proc) {
    Scheduler::cur_proc = p;
    return;
  }

  p->nxt = Scheduler::cur_proc->nxt;
  p->prev = Scheduler::cur_proc;
  p->nxt->prev = p;
  Scheduler::cur_proc->nxt = p;
}

Process *Scheduler::Reschedule(int remove) {
  if (!Scheduler::cur_proc) {
    return NULL;
  }

  Scheduler::cur_proc = Scheduler::cur_proc->nxt;

  if (remove) {
    // Removing last process from list
    if (Scheduler::cur_proc == Scheduler::cur_proc->nxt) {
      kfree(Scheduler::cur_proc);
      Scheduler::cur_proc = NULL;
      return NULL;
    }
    struct ProcQueueEntry *tmp = Scheduler::cur_proc->prev;
    tmp->prev->nxt = tmp->nxt;
    tmp->nxt->prev = tmp->prev;
    kfree(tmp);
  }

  return &Scheduler::cur_proc->proc;
}

Process *Scheduler::CurProc() { return &Scheduler::cur_proc->proc; }
