#ifndef PARTITIONFUNCTIONS
#define PARTITIONFUNCTIONS
#include"partitionFunctions.h"
#include"vdifile.h"
#include"vdiheader.h"
#include"vdifunctions.h"
#include"partitionData.h"
#include"mbr.h"

struct partitionFile * partitionFileOpen(struct vdifile *file,struct partitionEntry);
void partitionClose(struct partitionFile *partition);
int partitionSeek(struct partitionFile *f,int offset,int anchor,struct partitionEntry partitionInfo[]);
int mbrRead(struct vdifile *f, struct mbrSector& buf,int count);
int mbrSeek(struct vdifile *f,int offset,int anchor);
void displayPartitionInfo(struct mbrSector mbrData);
void displaySuperBlock(struct vdifile *f,struct dataBlock data,struct mbrSector mbrData);
#endif
