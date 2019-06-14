#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Process.h"
#include "Queue.h"

class Scheduler {
 public:
  static Process* Add(kentry_t entry, void* arg);
  static void Unblock(Queue*);
  static Process* Reschedule();
  static Process* GetCurProc();
  static void BlockCurProc(Queue*, uint64_t block_data_1,
                           uint64_t block_data_2);
  static Process ParentProc;

 private:
  static unsigned nxtPid;
  static struct QueueEntry* curProc;
  static Queue runQueue;
};

#endif
