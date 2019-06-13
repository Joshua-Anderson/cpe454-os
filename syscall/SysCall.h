
#ifndef __SYSCALL_H__
#define __SYSCALL_H__

class SysCall {
 public:
  static void ProcRun();
  static void ProcYield();
  static void ProcExit();
};

#endif
