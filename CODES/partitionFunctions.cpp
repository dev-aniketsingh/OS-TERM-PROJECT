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
void displayPartitionInfo(struct mbrSector mbrData){
  cout<<"--------------Partition Entry 0--------------"<<"\n";
  cout<<"Boot indicater : "<< (int)mbrData.partitionEntryInfo[0].bootIndicater<<"        Inactive"<<"\n";
  cout<<"First absolute sector CHS: ";
  for(int i=2;i>=0;i--){
    cout<<std::dec<<(int)mbrData.partitionEntryInfo[0].firstChs[i]<<"-";
  }
  cout<<"\n";
  cout<<" Partition Type : "<<(int)mbrData.partitionEntryInfo[0].partitionDescriptor<<"\n";
  cout<<"Last absolute sector CHS: ";
  cout<<std::dec<<(int)mbrData.partitionEntryInfo[0].lastChs[1]<<"-"<<(int)mbrData.partitionEntryInfo[0].lastChs[2]
      <<"-"<<(int)mbrData.partitionEntryInfo[0].lastChs[0]<<"\n";
  cout<<"Logical blocking : "<< mbrData.partitionEntryInfo[0].logicalBlocking<<endl;
  cout<<"Number of sectors in a partition : "<<std::dec<<mbrData.partitionEntryInfo[0].numberOfSectorInPartition<<"\n";
  cout<<"---------------------------------------------------------------------------------------"<<"\n";
  cout<<"\n\n"<<"--------------Partition Entry 1--------------"<<"\n";
  cout<<"Boot indicater : "<< (int)mbrData.partitionEntryInfo[1].bootIndicater<<"        Inactive"<<"\n";
  cout<<"First absolute sector CHS: ";
  for(int i=2;i>=0;i--){
    cout<<std::dec<<(int)mbrData.partitionEntryInfo[1].firstChs[i]<<"-";
  }
  cout<<"\n";
  cout<<" Partition Type : "<<(int)mbrData.partitionEntryInfo[1].partitionDescriptor<<"\n";
  cout<<"Last absolute sector CHS: ";
  cout<<std::dec<<(int)mbrData.partitionEntryInfo[1].lastChs[1]<<"-"<<(int)mbrData.partitionEntryInfo[1].lastChs[2]
      <<"-"<<(int)mbrData.partitionEntryInfo[1].lastChs[0]<<"\n";
  cout<<"Logical blocking : "<< mbrData.partitionEntryInfo[1].logicalBlocking<<endl;
  cout<<"Number of sectors in a partition : "<<std::dec<<mbrData.partitionEntryInfo[1].numberOfSectorInPartition<<"\n";
  cout<<"---------------------------------------------------------------------------------------"<<"\n";
  cout<<"\n\n"<<"--------------Partition Entry 2--------------"<<"\n";
  cout<<"Boot indicater : "<< (int)mbrData.partitionEntryInfo[2].bootIndicater<<"        Inactive"<<"\n";
  cout<<"First absolute sector CHS: ";
  for(int i=2;i>=0;i--){
    cout<<std::dec<<(int)mbrData.partitionEntryInfo[2].firstChs[i]<<"-";
  }
  cout<<"\n";
  cout<<" Partition Type : "<<(int)mbrData.partitionEntryInfo[2].partitionDescriptor<<"\n";
  cout<<"Last absolute sector CHS: ";
  cout<<std::dec<<(int)mbrData.partitionEntryInfo[2].lastChs[1]<<"-"<<(int)mbrData.partitionEntryInfo[2].lastChs[2]
      <<"-"<<(int)mbrData.partitionEntryInfo[2].lastChs[0]<<"\n";
  cout<<"Logical blocking : "<< mbrData.partitionEntryInfo[2].logicalBlocking<<endl;
  cout<<"Number of sectors in a partition : "<<std::dec<<mbrData.partitionEntryInfo[2].numberOfSectorInPartition<<"\n";
  cout<<"---------------------------------------------------------------------------------------"<<"\n";
  cout<<"\n\n"<<"--------------Partition Entry 3--------------"<<"\n";
  cout<<"Boot indicater : "<< (int)mbrData.partitionEntryInfo[3].bootIndicater<<"        Inactive"<<"\n";
  cout<<"First absolute sector CHS: ";
  for(int i=2;i>=0;i--){
    cout<<std::dec<<(int)mbrData.partitionEntryInfo[3].firstChs[i]<<"-";
  }
  cout<<"\n";
  cout<<" Partition Type : "<<(int)mbrData.partitionEntryInfo[3].partitionDescriptor<<"\n";
  cout<<"Last absolute sector CHS: ";
  cout<<std::dec<<(int)mbrData.partitionEntryInfo[3].lastChs[1]<<"-"<<(int)mbrData.partitionEntryInfo[3].lastChs[2]
      <<"-"<<(int)mbrData.partitionEntryInfo[3].lastChs[0]<<"\n";
  cout<<"Logical blocking : "<< mbrData.partitionEntryInfo[3].logicalBlocking<<endl;
  cout<<"Number of sectors in a partition : "<<std::dec<<mbrData.partitionEntryInfo[3].numberOfSectorInPartition;
  cout<<"---------------------------------------------------------------------------------------"<<"\n";

}
