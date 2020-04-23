#ifndef SUPERBLOCKFUNCTIONS
#define SUPERBLOCKFUNCTIONS
#include"ext2File.h"
#include"vdifile.h"
#include"mbr.h"

int readSuperBlock(struct ext2File * ext2,uint32_t blockNumber,struct vdifile * file,struct mbrSector mbr,int translationMapData[] );
void writeSuperBlock(struct ext2File* ext2,struct vdifile * file,struct mbrSector mbr,struct superBlock & buffer,int translationMapData[]);
int displaySuperBlock(struct ext2File* ext2);
int actualPage(int virtualAddress, struct vdifile *file, int translationMapData[]);
#endif
