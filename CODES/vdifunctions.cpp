#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<iostream>
#include <fstream>
#include"vdifunctions.h"
#include"vdiheader.h"
#include"vdifile.h"

using namespace std;

struct vdifile * vdiOpen(char *fn){
  int fileDescriptor;
  struct vdifile * file= (struct vdifile *) malloc(sizeof(struct vdifile));
  fileDescriptor= open(fn,O_RDWR);
  file->fileDescriptor= fileDescriptor;
  if(fileDescriptor==-1){
    cout<<"unable to open the given file "<<"\n";
    return NULL;
  }
  return file;
}

void vdiClose(struct vdifile *f){
  close(f->fileDescriptor);
}

int vdiRead(struct vdifile *f, void * buf, int count){
    int readBytes;
    readBytes= read(f->fileDescriptor,buf,count);
    f->pointerPosition= SEEK_CUR;
    if(readBytes==-1){
      cout<<"we are unable to read file !!"<<endl;
      return 0;
    }
    return readBytes;
}

int vdiSeek(struct vdifile *f,int offset,int anchor){
   int newOffset= lseek(f->fileDescriptor,offset,anchor);
   if(anchor== SEEK_SET && newOffset !=-1){
     f->pointerPosition= offset;
     return f->pointerPosition;
    }

    if(anchor == SEEK_CUR && newOffset !=-1){
     f->pointerPosition += offset;
     return f->pointerPosition;
   }
    if(anchor == SEEK_END && newOffset !=-1){
     f->pointerPosition = (int)f->header.diskSize+ offset;
     return f->pointerPosition;
   }
   return -1;

}
void dumpVDIHeader(struct vdifile * f){
    cout<<"Image Name: [";
    for(int i=0;i<39;i++){
      cout<<(char)f->header.imageName[i];
    }
    cout<<"\n"<<"]"<<"\n";
    cout<<endl;
    cout<<"Signature : 0x"<<hex <<f->header.signature<<"\n";
    cout<<"Version : "<< f->header.version<<"\n";
    cout<<"Header Size : 0x"<<std::hex<< f->header.headerSize<<"    "<<std::dec<<f->header.headerSize<<"\n";
    cout<<"Image Type : "<<f->header.imageType<<"\n";
    cout<<"Flags : "<<f->header.flag<<"\n";
    cout<<"Virtual CHS : "<<f->header.virtualChs<<"\n";
    cout<<"Sector Size : 0x"<<std::hex <<f->header.sectorSize<<"    "<<std::dec<<f->header.sectorSize<<"\n";
    cout<<"Logical CHS : "<<f->header.logicalChs<<"\n";
    cout<<"Sector Size : 0x"<<std::hex<< f->header.sectorSize<<"    "<<std::dec<<f->header.sectorSize<<"\n";
    cout<<"Map offset : 0x"<< std::hex<<f->header.mapOffset<<"    "<<std::dec<<f->header.mapOffset<<"\n";
    cout<<"Frame offset : 0x"<<std::hex<<f->header.frameOffset<<"    "<<std::dec<<f->header.frameOffset<<"\n";
    cout<<"Frame size: 0x"<<std::hex<<f->header.frameSize<<"    "<<std::dec<<f->header.frameSize<<"\n";
    cout<<"Extra Frame Size : 0x"<<std::hex<<f->header.extraFrameSize<<"    "<<std::dec<<f->header.extraFrameSize<<'\n';
    cout<<"Total Frames : 0x"<<std::hex<<f->header.totalFrame<<"    "<<std::dec<<f->header.totalFrame<<"\n";
    cout<<"Frames allocated : 0x"<<std::hex<<f->header.frameAllocated<<"    "<<std::dec<<f->header.frameAllocated<<"\n";
    cout<< "Disk Size : 0x"<<std::hex<<f->header.diskSize<<"    "<<std::dec<<f->header.diskSize<<"\n";
}
