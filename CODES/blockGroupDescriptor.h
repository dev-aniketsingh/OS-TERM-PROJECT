#ifndef BLOCKGROUPDESCRIPTOR
#define BLOCKGROUPDESCRIPTOR
#pragma pack(1)

struct blockGroupDescriptor{
  uint32_t bg_block_bitmap;
  uint32_t bg_inode_bitmap;
  uint32_t bg_inode_table;
  uint16_t bg_free_blocks;
  uint16_t bg_free_inodes;
  uint16_t bg_dirs_counts;
  uint16_t bg_pad;
  uint32_t bg_reserved[3];
};
#endif
