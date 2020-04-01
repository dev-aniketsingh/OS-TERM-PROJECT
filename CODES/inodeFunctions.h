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
int fetchInode(struct ext2File *f,struct vdifile *vdi,struct blockGroupDescriptor bg[], uint32_t iNum,
               struct inode& in,int offsetToSuperBlock,int translationMapData[],unsigned char inodeMetaData[]);
bool writeInode(struct ext2File *f,struct vdifile *vdi,struct blockGroupDescriptor bg[], uint32_t iNum,
                struct inode* in,int offsetToSuperBlock,int translationMapData[]);
int fetchBlockFromFile(struct inode * i, int bNum,struct superBlock sBlock,
                       struct ext2File * ext2,struct vdifile*file,struct mbrSector mbr,int translationMapData[]);
int * fetchBlock(struct ext2File * ext2,int blockNumber,struct vdifile*file,struct mbrSector mbr,int translationMapData[],int k);
bool writeBlockToFile(struct inode *i, int bNum,int offsetToGivenInode,int iNum,int blockSize,struct superBlock sBlock,
                     struct ext2File * ext2,struct vdifile*file,struct mbrSector mbr,int translationMapData[],struct blockGroupDescriptor bg[],
                     int offsetToSuperBlock,int * readBuffer);
bool inodeInUse(struct ext2File *f,unsigned char inodeBitMap[]);
void allocateInode(int & indexToFreeInode,unsigned char inodeBitMap[]);
void freeInode(int iNum,unsigned char inodeBitMap[]);
unsigned char * fetchInodeBitMap(struct ext2File *f,struct vdifile *vdi,struct blockGroupDescriptor bg[], uint32_t iNum,
                                 int offsetToSuperBlock,int translationMapData[]);
void displayInode(struct inode in);
int allocateBlock(struct ext2File *f,struct vdifile *vdi,struct blockGroupDescriptor bg[], uint32_t iBlockNumber,
                  int offsetToSuperBlock,int translationMapData[]);
bool writeBlock(struct ext2File * ext2,int blockNumber,struct vdifile*file,struct mbrSector mbr,int translationMapData[],int *buffer);
