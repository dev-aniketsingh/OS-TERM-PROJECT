#ifndef PARTITIONFUNCTIONS
#define PARTITIONFUNCTIONS
#include"partitionFunctions.h"
#include"vdifile.h"
#include"vdiheader.h"
#include"vdifunctions.h"
#include"mbr.h"

struct mbrSector * mbrOpen(struct vdifile *file,struct partitionEntry);
void mbrClose(struct mbrSector *f);
int mbrRead(struct vdifile *f, struct mbrSector& buf,int count);
int mbrWrite(struct mbrSector *f,void * buf, int count);
int mbrSeek(struct vdifile *f,int offset,int anchor);
void displayPartitionInfo(struct mbrSector mbrData);
#endif
