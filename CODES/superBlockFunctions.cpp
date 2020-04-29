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
#include"superblockFunctions.h"
#include"superblock.h"
#include"ext2File.h"
#include"blockGroupDescriptor.h"
#include<iostream>
using namespace std;
int readSuperBlock(struct ext2File * ext2,uint32_t blockNumber,struct vdifile*file,struct mbrSector mbr,int translationMapData[]){
  /*
      This given codes can be used to located the physical page of the vdi dynamic file
    */
    int finalResult;
    int virtualAddress= mbr.partitionEntryInfo[0].logicalBlocking*512+1024;
    int physicalAddress= actualPage(virtualAddress,file,translationMapData);
    vdiSeek(file,physicalAddress,SEEK_SET);
    finalResult=vdiRead(file,&(ext2->superblock),sizeof(ext2->superblock));
    if(finalResult!=-1){
      return -1;
    }
    return 0;
}
int actualPage(int virtualAddress, struct vdifile *file, int translationMapData[]){
      int physicalPageNumber,
      offsetToPhysicalPage,
      physicalAddress;
  physicalPageNumber= translationMapData[virtualAddress/file->header.frameSize];
  if(physicalPageNumber==-1 || physicalPageNumber==-2){
    lseek(file->fileDescriptor,0,SEEK_END);
    write(file->fileDescriptor,0,file->header.frameSize);
    translationMapData[virtualAddress/file->header.frameSize]= file->header.frameAllocated;
    file->header.frameAllocated++;
  }
  offsetToPhysicalPage= (virtualAddress)%file->header.frameSize;
  physicalAddress= file->header.frameOffset+physicalPageNumber*file->header.frameSize+offsetToPhysicalPage;
  return physicalAddress;
}
int displaySuperBlock(struct ext2File* ext2){
  cout<<"Super Block Contents: "<<"\n";
  cout<<"Number of inodes : "<<std::dec<< ext2->superblock.s_inodes_count<<"\n";
  cout<<"Number of blocks : "<<std::dec<< ext2->superblock.s_blocks_count<<"\n";
  cout<<"Number of reserved blocks : "<<std::dec<< ext2->superblock.s_r_blocks_count<<"\n";
  cout<<"Number of free blocks : "<<std::dec<< ext2->superblock.s_free_blocks_count<<"\n";
  cout<<"Number of free inodes : "<<std::dec<< ext2->superblock.s_free_inodes_count<<"\n";
  cout<<"First data block : "<<std::dec<< ext2->superblock.s_first_data_block<<"\n";
  cout<<"Log block size  : "<<std::dec<< ext2->superblock.s_log_block_size<<"\n";
  cout<<"Log fragment size : "<<std::dec<< ext2->superblock.s_log_frag_size<<"\n";
  cout<<"Blocks per group : "<<std::dec<< ext2->superblock.s_blocks_per_group<<"\n";
  cout<<"Fragments per group : "<<std::dec<< ext2->superblock.s_frags_per_group<<"\n";
  cout<<"Inodes per group : "<<std::dec<< ext2->superblock.s_inodes_per_group<<"\n";
  cout<<"Last mount time : "/*<<std::dec*/<< ext2->superblock.s_mtime<<"\n";
  cout<<"Last write time : "/*<<std::dec*/<< ext2->superblock.s_wtime<<"\n";
  cout<<"Mount count : "<<std::dec<< ext2->superblock.s_mnt_count<<"\n";
  cout<<"Max mount count : "<<std::dec<< ext2->superblock.s_max_mnt_count<<"\n";
  cout<<"Magic Number : "<<std::hex<< ext2->superblock.s_magic<<"\n";
  cout<<"State : "<<std::dec<< ext2->superblock.s_state<<"\n";
  cout<<"Error Processing : "<<std::dec<< ext2->superblock.s_errors<<"\n";
  cout<<"Revision Level : "<<std::dec<< ext2->superblock.s_rev_level<<"\n";
  cout<<"Last system check : "<<std::dec<< ext2->superblock.s_lastcheck<<"\n";
  cout<<"Check Interval : "<<std::dec<< ext2->superblock.s_checkinterval<<"\n";
  cout<<"OS creator : "<<std::dec<< ext2->superblock.s_creator_os<<"\n";
  cout<<"Default reserve UID : "<<std::dec<< ext2->superblock.s_def_resuid<<"\n";
  cout<<"Default reserve GID : "<<std::dec<< ext2->superblock.s_def_resgid<<"\n";
  cout<<"First Inode Number : "<<std::dec<< ext2->superblock.s_first_ino<<"\n";
  cout<<"Inode Size : "<<std::dec<< ext2->superblock.s_inode_size<<"\n";
  cout<<"Block group Number : "<<std::dec<< ext2->superblock.s_block_group_nr<<"\n";
  cout<<"Feature Compatibility bits : "<<std::dec<< ext2->superblock.s_feature_compat<<"\n";
  cout<<"Feature incompatibility bits : "<<std::dec<< ext2->superblock.s_feature_incompat<<"\n";
  cout<<"Feature read/only Compatibility bits : "<<std::dec<< ext2->superblock.s_feature_ro_compat<<"\n";
  cout<<"UUID : ";
  for(uint8_t x: ext2->superblock.s_uuid){
    cout<<std::hex<<(int)x;
  }
  cout<<"\n";
  cout<<"Volume Name : ";
  for(uint8_t x: ext2->superblock.s_volume_name){
    cout<<std::hex<<(int)x;
  }
  cout<<"\n";
  cout<<"Last Mount : "<<std::dec<< ext2->superblock.s_last_mounted<<"\n";
  cout<<"Algorithm bitmap : "<<std::dec<< ext2->superblock.s_algo_bitmap<<"\n";
  cout<<"Number of blocks to preallocate : "<< (int)ext2->superblock.s_prealloc_blocks<<"\n";
  cout<<"Number of blocks to preallocate for directories : "<<(int) ext2->superblock.s_prealloc_dir_blocks<<"\n";
  cout<<"Journal UUID : ";
  for(uint8_t x: ext2->superblock.s_journal_uuid){
    cout<<std::hex<<(int)x;
  }
  cout<<"\n";
  cout<<"Journal inode Number : "<<std::dec<< ext2->superblock.s_journal_inum<<"\n";
  cout<<"Journal Device Number : "<<std::dec<< ext2->superblock.s_journal_dev<<"\n";
  cout<<"Journal Last orphan inode Number : "<<std::dec<< ext2->superblock.s_last_orphan<<"\n";
  cout<<"Default hash version : "<<(int) ext2->superblock.s_def_hash_version<<"\n";
  cout<<"Default mount option bitmap : "<<std::hex<< ext2->superblock.s_default_mount_opts<<"\n";
  cout<<"First Meta Block Group : "<<std::dec<< ext2->superblock.s_first_meta_bg<<"\n";

}

void writeSuperBlock(struct ext2File* ext2,struct vdifile * file,struct mbrSector mbr,struct superBlock& buffer,int translationMapData[]){
  int offset= mbr.partitionEntryInfo[0].logicalBlocking*512+1024;
  int physicalAddress= actualPage(offset,file, translationMapData);
  vdiSeek(file,physicalAddress,SEEK_SET);
  int writeBytes=write(file->fileDescriptor,&buffer,sizeof(buffer));
  if(writeBytes!=sizeof(buffer)){
    cout<<"unable to write superBlock successfully "<<endl;
  }
}
