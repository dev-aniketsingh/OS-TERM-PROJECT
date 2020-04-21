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
bool writeBlockToFile(struct inode *, int,int,int,int,struct superBlock,struct ext2File *,struct vdifile*,struct mbrSector,
                      int[],struct blockGroupDescriptor[],int,int *);
bool inodeInUse(struct ext2File *,unsigned char [], int &);
void allocateInode(int & ,unsigned char [],struct ext2File * ext2);
void freeInode(int,unsigned char[]);
bool fetchInodeBitMap(struct ext2File *,struct vdifile *,struct blockGroupDescriptor[], uint32_t,int,int[],unsigned char []);
void displayInode(struct inode in);
int allocateBlock(struct ext2File *,struct vdifile *,struct blockGroupDescriptor[], uint32_t,int,int[]);
bool writeBlock(struct ext2File *,int,struct vdifile*,struct mbrSector,int[],int *);
