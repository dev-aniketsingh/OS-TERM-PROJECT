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
#include"inodeFunctions.h"
#include<string>
using namespace std;

int fetchInode(struct ext2File *f,struct vdifile *vdi,struct blockGroupDescriptor bg[], uint32_t iNum,
               struct inode& in,int offsetToSuperBlock,int translationMapData[],unsigned char inodeMetaData[]){
    int readInodeBytes,
        blockSize,
        blockGroupNumber,
        offsetToGivenInode;
    blockSize=1024<<f->superblock.s_log_block_size;
    blockGroupNumber= (iNum-1)/f->superblock.s_inodes_per_group;
    offsetToGivenInode=blockGroupNumber*(blockSize)* f->superblock.s_blocks_per_group+offsetToSuperBlock-vdi->header.frameOffset+
                      (bg[blockGroupNumber].bg_inode_table-1-blockGroupNumber*f->superblock.s_blocks_per_group)*
                      (blockSize)+((iNum-1)%f->superblock.s_inodes_per_group)*f->superblock.s_inode_size;
    int physicalAddress= actualPage(offsetToGivenInode,vdi,translationMapData);
    vdiSeek(vdi,physicalAddress,SEEK_SET);
    readInodeBytes= vdiRead(vdi,&in,sizeof(in));
    vdiSeek(vdi,physicalAddress,SEEK_SET);
    vdiRead(vdi,inodeMetaData,128);
    return readInodeBytes;
}

bool inodeInUse(struct ext2File *f,unsigned char inodeBitMap[], int & indexToFreeInode){
    int oneByte;
    bool isAllocated= true;
    for(int i=0;i<(1024<<f->superblock.s_log_block_size);i++){
    oneByte= inodeBitMap[i];
    for(int j=0;j<8;j++){
      isAllocated= (oneByte>>j & 0x1);
      if(!isAllocated){
        indexToFreeInode= i*8 +(7-j);
        return (!isAllocated);
      }
    }
  }
  return isAllocated;
}

unsigned char * fetchInodeBitMap(struct ext2File *f,struct vdifile * vdi, struct blockGroupDescriptor bg[], uint32_t iNum, int offsetToSuperBlock){
  int readBitBytes,
      blockSize,
      blockGroupNumber,
      offsetInodeBitMap;
  blockSize=1024<<f->superblock.s_log_block_size;
  blockGroupNumber= (iNum-1)/f->superblock.s_inodes_per_group;
  offsetInodeBitMap=blockGroupNumber*(blockSize)* f->superblock.s_blocks_per_group+offsetToSuperBlock+
                    (bg[blockGroupNumber].bg_inode_bitmap-1-blockGroupNumber*f->superblock.s_blocks_per_group)*(blockSize);
  unsigned char inodeBitMap[blockSize];
  vdiSeek(vdi,offsetInodeBitMap,SEEK_SET);
  readBitBytes= vdiRead(vdi,inodeBitMap,sizeof(inodeBitMap));
  return inodeBitMap;
}
void allocateInode(int & indexToFreeInode,unsigned char inodeBitMap[]){
    int byteOffset= (indexToFreeInode-1)/8;
    int offsetIntoByte= (indexToFreeInode-1)%8;
    int byteData= inodeBitMap[byteOffset];
    inodeBitMap[byteOffset]= 0x1<<(7-offsetIntoByte) | byteData;
}
void freeInode(int iNum,unsigned char inodeBitMap[]){
    int byteOffset= (iNum-1)/8;
    int offsetIntoByte= 7-(iNum-1)%8;
    int byteData= inodeBitMap[byteOffset];
    inodeBitMap[byteOffset]= (byteData>>offsetIntoByte ^ 0x1);
}
void displayInode(struct inode in){
  cout<<"\n"<<"Mode : "<<std::oct<<in.i_mode<<"\n";
  cout<<"Size : "<<std::dec<<in.i_size<<"\n";
  cout<<"Blocks : "<<std::dec<<in.i_blocks<<"\n";
  cout<<"UID/GID : "<<std::dec<<in.i_uid<<"\n";
  cout<<"Links : "<<std::dec<<in.i_links_count<<"\n";
  cout<<"Last created : "<<std::dec<<in.i_ctime<<" Seconds since 1970 janurary 1st"<<"\n";
  cout<<"Last access : "<<std::dec<<in.i_atime<<"  Seconds since 1970 janurary 1st"<<"\n";
  cout<<"Last Modification : "<<std::dec<<in.i_mtime<<"  Seconds since 1970 janurary 1st"<<"\n";
  cout<<"Deleted : "<<std::dec<<in.i_dtime<<"  Seconds since 1970 janurary 1st"<<"\n";
  cout<<"Flags : "<<std::dec<<in.i_flags<<"\n";
  cout<<" File version : "<<std::dec<<in.i_generation<<"\n";
  cout<<"ACL blocks : "<<std::dec<<in.i_file_acl<<"\n";
  cout<<"Direct Blocks: "<<"\n";
  cout<<"0-3 : "<<"\n";
  for(int i=0;i<4;i++){
    cout<<std::dec<<in.i_block[i]<<" ";
  }
  cout<<"\n"<<"4-7 : "<<"\n";
  for(int i=4;i<8;i++){
    cout<<std::dec<<in.i_block[i]<<" ";
  }
  cout<<"\n"<<"8-11 : "<<"\n";
  for(int i=8;i<12;i++){
    cout<<std::dec<<in.i_block[i]<<" ";
  }
  cout<<"\n"<<"Single Indirect Block : "<<std::dec<<in.i_block[12]<<"\n";
  cout<<"Double Indirect Block : "<<std::dec<<in.i_block[13]<<"\n";
  cout<<"Triple Indirect Block : "<<std::dec<<in.i_block[14]<<"\n";


}
