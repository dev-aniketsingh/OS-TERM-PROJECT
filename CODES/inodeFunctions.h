#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include <fstream>
#include<iostream>
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
int fetchInode(struct ext2File *f,struct vdifile *vdi,struct blockGroupDescriptor bg[], uint32_t iNum,struct inode& in,int offsetToSuperBlock,int translationMapData[],unsigned char inodeMetaData[]);
int writeInode(struct ext2File *f,struct blockGroupDescriptor bg[], uint32_t iNum, struct inode in);
bool inodeInUse(struct ext2File *f,unsigned char inodeBitMap[]);
void allocateInode(int & indexToFreeInode,unsigned char inodeBitMap[]);
void freeInode(int iNum,unsigned char inodeBitMap[]);
unsigned char * fetchInodeBitMap(struct ext2File *f,struct vdifile *vdi,struct blockGroupDescriptor bg[], uint32_t iNum,int offsetToSuperBlock,int translationMapData[]);
void displayInode(struct inode in);