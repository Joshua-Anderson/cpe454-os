#ifndef _CHAR_DEV_H__
#define _CHAR_DEV_H__

class CharDev {
 public:
  virtual void Init() = 0;
  virtual char GetChar() = 0;
};

#endif
