#ifndef VDIFILE
#define VDIFILE
#include"vdiheader.h"
#pragma pack(1)
struct vdifile{
  int pageSize;
  int fileDescriptor;
  vdiheader header;
  int pointerPosition;
};

#endif
