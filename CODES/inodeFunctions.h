#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include <fstream>
#include"vdiheader.h"
#include"vdifile.h"
#include"vdifunctions.h"
#include"partitionData.h"
#include"mbr.h"
#include"partitionFunctions.h"
#include"superblock.h"
#include"superblockFunctions.h"
#include"blockGroupDescriptor.h"
#include"ext2File.h"
#include"inode.h"
#include"directoryEntry.h"

bool fetchInode(struct ext2File*,struct vdifile*,struct blockGroupDescriptor[], uint32_t,struct inode&,int,int[],unsigned char[]);
bool writeInode(struct ext2File *,struct vdifile *,struct blockGroupDescriptor[], uint32_t,struct inode*,int,int[]);
bool fetchBlockFromFile(struct inode *, int,struct superBlock,struct ext2File *,struct vdifile*,struct mbrSector,int[],unsigned char[]);
void fetchBlock(struct ext2File *,int,struct vdifile*,struct mbrSector,int[],int,int[]);
bool writeBlockToFile(struct inode *i, int bNum,int iNum,int blockSize,struct superBlock sBlock,
                     struct ext2File *,struct vdifile*,struct mbrSector,int[],struct blockGroupDescriptor[],int,unsigned char *,unsigned char[],int&,int);
bool inodeInUse(struct ext2File *,unsigned char [], int &);
void allocateInode(int & indexToFreeInode,unsigned char inodeBitMap[],struct blockGroupDescriptor bg[],struct ext2File * ext2,int blockGroup);
void freeInode(int,unsigned char[]);
bool fetchInodeBitMap(struct ext2File *,struct vdifile *,struct blockGroupDescriptor[],int,int,int[],unsigned char [],int&);
void displayInode(struct inode in);
int allocateBlock(struct ext2File *f,struct blockGroupDescriptor bg[],unsigned char blockBitMap[],int blockGroup);
bool writeBlock(struct ext2File *,int,struct vdifile*,struct mbrSector,int[],int *,int);
bool fetchBlockBitMap(struct ext2File *f,struct vdifile * vdi, struct blockGroupDescriptor bg[],int blockGroupNumber,
                      int offsetToSuperBlock,int translationMapData[],unsigned char blockBitMap[],int&);
