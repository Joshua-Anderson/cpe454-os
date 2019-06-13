#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Process.h"

struct ProcQueueEntry {
  Process proc;
  struct ProcQueueEntry* nxt;
  struct ProcQueueEntry* prev;
};

class Scheduler {
 public:
  static Process* Add(kentry_t entry, void* arg);
  static Process* Reschedule(int remove);
  static Process* CurProc();
  static Process parent_proc;

 private:
  static unsigned nxt_pid;
  static struct ProcQueueEntry* cur_proc;
};

#endif
