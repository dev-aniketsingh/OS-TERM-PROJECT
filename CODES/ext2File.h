#ifndef EXT2FILE
#define EXT2FILE
#include"superblock.h"
#include"blockGroupDescriptor.h"
#pragma pack(1)

struct ext2File{
  struct superBlock superblock;
};

#endif
