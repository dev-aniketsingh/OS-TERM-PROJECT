#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<iostream>
#include<stdio.h>
#include"partitionFunctions.h"
#include"vdifile.h"
#include"vdiheader.h"
#include"vdifunctions.h"
#include"mbr.h"
using namespace std;

struct mbrSector * mbrOpen(struct vdifile * file,struct partitionEntry partitionInfo){


}

void mbrClose(struct mbrSector *f){
  f=NULL;
  free(f);
}

int mbrRead(struct vdifile *f,struct mbrSector& buf,int count){
  int readBytes=vdiRead(f,&buf,sizeof(buf));
  f->pointerPosition= SEEK_CUR;
  if(readBytes==sizeof(buf)){
    cout<<"Your all data has been read "<<"\n";
  }
  else{
    cout<<"There are some data that werenot read"<<readBytes<<"\n";
  }
  return readBytes;
}

int mbrWrite(struct mbrSector *f,void * buf, int count){



}

int mbrSeek(struct vdifile *f,int offset,int anchor){
    if(anchor== SEEK_SET && offset<=f->header.frameOffset+sizeof(struct mbrSector)&& offset>=f->header.frameOffset){
      f->pointerPosition=lseek(f->fileDescriptor,offset,anchor);
      return f->pointerPosition;
    }
    if(anchor== SEEK_CUR && f->header.frameOffset+sizeof(struct mbrSector)>=SEEK_CUR+offset && SEEK_CUR+offset>=f->header.frameOffset){
      f->pointerPosition=lseek(f->fileDescriptor,offset,anchor);
      return f->pointerPosition;
    }
    if(anchor== SEEK_END && f->header.frameOffset+sizeof(struct mbrSector)>=SEEK_END+offset && SEEK_END+offset>=f->header.frameOffset ){
      f->pointerPosition=lseek(f->fileDescriptor,offset,anchor);
      return f->pointerPosition;
    }
    return -1;
}
