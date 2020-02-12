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
using namespace std;
void displayTranslationMap(struct vdifile * file);
void displayUUID(struct vdifile*,struct UUID*);

struct UUID {
    uint32_t
        timeLow;
    uint16_t
        timeMid,
        timeHigh,
        clock;
    uint8_t
        node[6];
};
static char *uuid2ascii(struct UUID *);
static char
    uuidStr[40];

int main(int argc, char* argv[]){
  struct vdifile * file ;
  if(argc < 2) {
    cout <<"File not included \n";
    return 1;
  }
  int mbrReadBytes;
  struct mbrSector mbrData;
  struct dataBlock data;
  file = vdiOpen(argv[1]);
 /*
  it reads the vdi header
 */
  vdiRead(file,&(file->header),sizeof(file->header));
  struct UUID* id= (struct UUID *)malloc(sizeof(struct UUID));
  dumpVDIHeader(file);
  displayUUID(file,id);
  displayTranslationMap(file);

   /* Change the position of file descriptor pointer and read the mbr data*/
   mbrSeek(file,file->header.frameOffset,SEEK_SET);
   mbrReadBytes=mbrRead(file,mbrData,sizeof(mbrData));
   displayPartitionInfo(mbrData);
   vdiSeek(file,file->header.frameOffset+mbrData.partitionEntryInfo[0].logicalBlocking*512+1024,SEEK_SET);
   vdiRead(file,&data,sizeof(data));
   displaySuperBlock(file,data,mbrData);
   /*
    This reads the super block in located in the partittion
   */
  struct ext2File * ext2 = (struct ext2File *) malloc(sizeof(ext2));
  readSuperBlock(ext2,0,file,mbrData);
  displaySuperBlock(ext2);
  /*offset to superBlock*/
  int blockSize= 1024<<(ext2->superblock.s_log_block_size);
  int totalBlockGroup= (ext2->superblock).s_blocks_count/(ext2->superblock).s_blocks_per_group;
  struct blockGroupDescriptor table[totalBlockGroup];
  vdiRead(file,table,sizeof(table));
  cout<<"Block Bit Map"<<"\t"<<"Inode Bitmap"<<"\t"<<"Innode Table"<<"\t"<<"Free Blocks"<<"\t"<<"Free Inodes"<<"\t"<<"Used Directory"<<"\n";
  for(int i=0;i<totalBlockGroup;i++){
    cout<<table[i].bg_block_bitmap<<"\t\t";
    cout<<table[i].bg_inode_bitmap<<"\t\t";
    cout<<table[i].bg_inode_table<<"\t\t";
    cout<<table[i].bg_free_blocks<<"\t\t";
    cout<<table[i].bg_free_inodes<<"\t\t";
    cout<<table[i].bg_dirs_counts<<"\t\t";
    cout<<endl;

  }

   return 0;
}

static char *uuid2ascii(struct UUID *uuid) {
    sprintf(uuidStr,"%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x",
            uuid->timeLow,uuid->timeMid,uuid->timeHigh,uuid->clock,
            uuid->node[0],uuid->node[1],uuid->node[2],uuid->node[3],
            uuid->node[5],uuid->node[5]);
    return uuidStr;
}
void displayTranslationMap(struct vdifile * file){
  int count=0;
  for(int z: file->header.translationMap ){
      cout<<hex<<z<<"\t";
      count++;
      if(count==16){
          cout<<endl;
          count=0;
      }
  }
}
void displayUUID(struct vdifile* file,struct UUID *id){
  vdiRead(file,id,sizeof(struct UUID));
  uuid2ascii(id);
  cout<<"UUID : "<<uuidStr<<"\n";
  vdiRead(file,id,sizeof(struct UUID));
  uuid2ascii(id);
  cout<<"Last Snap UUID: "<<uuidStr<<"\n";
  vdiRead(file,id,sizeof(struct UUID));
  uuid2ascii(id);
  cout<<"Link UUID :"<<uuidStr<<"\n";
  vdiRead(file,id,sizeof(struct UUID));
  uuid2ascii(id);
  cout<<"Parent UUID :"<<uuidStr<<"\n";
}
