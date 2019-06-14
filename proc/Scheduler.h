#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Process.h"
#include "Queue.h"

class Scheduler {
 public:
  static Process* Add(kentry_t entry, void* arg);
  static Process* Reschedule();
  static Process* GetCurProc();
  static Process ParentProc;

 private:
  static unsigned nxtPid;
  static struct QueueEntry* curProc;
  static Queue runQueue;
};

#endif
