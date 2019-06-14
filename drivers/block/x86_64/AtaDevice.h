#ifndef __ATA_DEVICE_H__
#define __ATA_DEVICE_H__

#include <stdint.h>

#include "../BlockDevice.h"

#define ATA_BLOCK_SIZE 512

class AtaDevice : public BlockDevice {
 public:
  AtaDevice();
  uint32_t BlockSize() { return ATA_BLOCK_SIZE; };
  int ReadBlock(uint64_t blockNum, void *dst);
};

#endif
