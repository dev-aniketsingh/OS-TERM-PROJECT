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
#include"inode.h"
#include<string>
#include<vector>
using namespace std;
void displayTranslationMap(struct vdifile * file);
void displayUUID(struct vdifile*,struct UUID*);
void displayInodeMeta(unsigned char inodeMetaData[],int iNum);
struct __attribute__((packed)) UUID {
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
  struct UUID id;
  dumpVDIHeader(file);
  displayUUID(file,&id);
  displayTranslationMap(file);

   /* Change the position of file descriptor pointer and read the mbr data*/
   mbrSeek(file,file->header.frameOffset,SEEK_SET);
   mbrReadBytes=mbrRead(file,mbrData,sizeof(mbrData));
   displayPartitionInfo(mbrData);
   int offsetToSuperBlock= file->header.frameOffset+mbrData.partitionEntryInfo[0].logicalBlocking*512+1024;
   vdiSeek(file,offsetToSuperBlock,SEEK_SET);
   vdiRead(file,&data,sizeof(data));
   displaySuperBlock(file,data,mbrData);
   /*
     It reads the translation map , which can be used to translate the virtual page to physical page in dynamic vdi file
   */
   int offsetToTranslationMap,
       readBytesOfTranslationMap,
       count=0;
   int translationMapData[4*file->header.totalFrame];
   offsetToTranslationMap=vdiSeek(file,file->header.mapOffset,SEEK_SET);
   readBytesOfTranslationMap= vdiRead(file,translationMapData,sizeof(translationMapData));

   /*
    This reads the super block in located in the partittion
   */
  struct ext2File ex;
  struct ext2File * ext2 = &ex;
  readSuperBlock(ext2,0,file,mbrData,translationMapData);
  displaySuperBlock(ext2);
  /*
    This given line of codes can be used to read the block group descriptor table data
  */
  int blockSize= 1024<<(ext2->superblock.s_log_block_size);
  int totalBlockGroup= ((ext2->superblock).s_blocks_count-ext2->superblock.s_first_data_block)/(ext2->superblock).s_blocks_per_group;
  if(((ext2->superblock).s_blocks_count-ext2->superblock.s_first_data_block)%(ext2->superblock).s_blocks_per_group>0){
     totalBlockGroup++;
  }
  struct blockGroupDescriptor table[totalBlockGroup];
  if(blockSize==4096){
    int offsetToBlockGroup= offsetToSuperBlock-1024+blockSize;
    vdiSeek(file,offsetToBlockGroup,SEEK_SET);
  }
  vdiRead(file,table,sizeof(table));
  cout<<"Block Group NO.\t"<<"Block Bit Map"<<"\t"<<"Inode Bitmap"<<"\t"<<"Innode Table"<<"\t"<<"Free Blocks"<<"\t"<<"Free Inodes"<<"\t"<<"Used Directory"<<"\n";
/*.
  it can be used to display the content of the group descriptor of the registered block group
*/
  for(int i=0;i<totalBlockGroup;i++){
    cout<<i<<"\t\t";
    cout<<table[i].bg_block_bitmap<<"\t\t";
    cout<<table[i].bg_inode_bitmap<<"\t\t";
    cout<<table[i].bg_inode_table<<"\t\t";
    cout<<table[i].bg_free_blocks<<"\t\t";
    cout<<table[i].bg_free_inodes<<"\t\t";
    cout<<table[i].bg_dirs_counts<<"\t\t";
    cout<<endl;
  }
  /*
    This given code is used to read the given inode
  */
  struct inode  in;
  int readInodeBytes;
  unsigned char inodeMetaData[128];
  readInodeBytes= fetchInode(ext2,file,table,2,in,offsetToSuperBlock,translationMapData,inodeMetaData);
  displayInodeMeta(inodeMetaData,2);
  displayInode(in);
  fetchInode(ext2,file,table,12,in,offsetToSuperBlock,translationMapData,inodeMetaData);
  displayInodeMeta(inodeMetaData,12);
  displayInode(in);
  /*
  This given lines code can be used to fetch the data block inside the file
  */
  int blockNumber=fetchBlockFromFile(&in,14,ext2->superblock,ext2,file,mbrData,translationMapData);
  if(blockNumber!=0){
    unsigned char buff[blockSize];
    int offsetToGivenBlock=mbrData.partitionEntryInfo[0].logicalBlocking*512+blockNumber*blockSize;
    int physicalAddress= actualPage(offsetToGivenBlock,file,translationMapData);
    vdiSeek(file,physicalAddress,SEEK_SET);
    vdiRead(file,buff,blockSize);
    int count=0;
    cout<<"Block No. 14 from the File"<<endl;
    for(unsigned char x: buff){
      cout<<std::hex<<(int)x<<"\t";
      count++;
      if(count==16){
        cout<<endl;
        count=0;
      }
    }
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
void displayInodeMeta(unsigned char * inodeMetaData,int iNum){
    int j=0;
    cout<<endl<<"----------------------------------"<<"Inode :"<<iNum<<"--------------------------------------------------------------------------------------"<<endl;

    for(int i=0;i<128;i++){
      cout<<std::hex<<(int)*(inodeMetaData+i)<<"\t";
      j++;
      if(j==16){
        cout<<endl;
        j=0;
      }
    }
    cout<<endl<<"------------------------------------------------------------------------------------------------------------------------"<<endl;

}
