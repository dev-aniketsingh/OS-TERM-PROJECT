#ifndef SUPERBLOCKFUNCTIONS
#define SUPERBLOCKFUNCTIONS
#include"ext2File.h"
#include"vdifile.h"
#include"mbr.h"

int readSuperBlock(struct ext2File * ext2,uint32_t blockNumber,struct vdifile * file,struct mbrSector mbr,int translationMapData[] );
int writeSuperBlock(struct ext2File* ext2, uint32_t blockNumber,struct vdifile * file,struct mbrSector mbr);
int displaySuperBlock(struct ext2File* ext2);
#endif
