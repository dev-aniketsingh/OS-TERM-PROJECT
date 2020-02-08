#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<iostream>
#include"vdiheader.h"
#include"vdifile.h"
#include"vdifunctions.h"
#include"partitionData.h"
#include"mbr.h"
#include"partitionFunctions.h"
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

int main(){
   struct vdifile * file ;
   int mbrReadBytes;
   struct mbrSector mbrData;
   file = vdiOpen("/home/sagar/5806.public/Test-fixed-1k.vdi");
   vdiRead(file,&(file->header),sizeof(file->header));
   struct UUID* id= (struct UUID *)malloc(sizeof(struct UUID));
   dumpVDIHeader(file);
   displayUUID(file,id);
   displayTranslationMap(file);

   /* Change the position of file descriptor pointer and read the mbr data*/
   mbrSeek(file,file->header.frameOffset,SEEK_SET);
   mbrReadBytes=mbrRead(file,mbrData,sizeof(mbrData));
   displayPartitionInfo(mbrData);
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
