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
#include<iostream>
using namespace std;

int readSuperBlock(struct ext2File * ext2,uint32_t blockNumber,struct vdifile*file,struct mbrSector mbr){
  /*
   This only works in the case of fixed vdi file
  */
    int finalResult;
    int offsetToSuperBlock= blockNumber*1024+1024+mbr.partitionEntryInfo[0].logicalBlocking*512+file->header.frameOffset;
    vdiSeek(file,offsetToSuperBlock,SEEK_SET);
    finalResult=vdiRead(file,&(ext2->superblock),sizeof(ext2->superblock));
    if(finalResult!=-1){
      return -1;
    }
    return 0;
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
  cout<<"Last mount time : "<<std::dec<< ext2->superblock.s_last_mounted<<"\n";
  cout<<"Last write time : "<<std::dec<< ext2->superblock.s_wtime<<"\n";
  cout<<"Mount count : "<<std::dec<< ext2->superblock.s_mtime<<"\n";
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
  cout<<"First Inode Number : "<<std::dec<< ext2->superblock.s_first_ino<<"\n;
  cout<<"Inode Size : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Block group Number : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Feature Compatibility bits : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Feature incompatibility bits : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Feature read/only Compatibility bits : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"UUID : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Volume Name : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Last Mount : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Algorithm bitmap : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Number of blocks to preallocate : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Number of blocks to preallocate for directories : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Journal UUID : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Journal inode Number : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Journal Device Number : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"Journal Last orphan inode Number : "<<std::dec<< ext2->superblock.s_inodes_count;
  cout<<"First Meta Block Group : "<<std::dec<< ext2->superblock.s_inodes_count;

}

int writeSuperBlock(struct ext2File* ext2, uint32_t blockNumberSS,struct vdifile * file,struct mbrSector mbr){


}