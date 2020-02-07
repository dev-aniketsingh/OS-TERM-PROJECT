#ifndef VDIFUNCTIONS
#define VDIFUNCTIONS
#include"vdifile.h"
#include"vdiheader.h"

struct vdifile * vdiOpen(char * fn);
void vdiClose(struct vdifile *f);
int vdiRead(struct vdifile *f , void * buf, int count);
int vdiWrite(struct vdifile *f, void * buf, int count);
int vdiSeek(struct vdifile *f, int offset, int anchor);
void dumpVDIHeader(struct vdifile * f);
#endif
