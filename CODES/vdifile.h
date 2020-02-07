#ifndef VDIFILE
#define VDIFILE
#include"vdiheader.h"

struct vdifile{
  int pageSize;
  int fileDescriptor;
  vdiheader header;
  int pointerPosition;
};

#endif
