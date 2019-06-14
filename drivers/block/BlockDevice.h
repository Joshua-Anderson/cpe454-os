#ifndef __BLOCK_DEVICE_H__
#define __BLOCK_DEVICE_H__

#include <stdint.h>

class BlockDevice {
 public:
  virtual uint32_t BlockSize() = 0;
  virtual int ReadBlock(uint64_t blockNum, void *dst) = 0;
};

#endif
