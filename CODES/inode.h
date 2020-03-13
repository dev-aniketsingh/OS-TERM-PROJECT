#ifndef INODE
#define INODE
#pragma pack(1)

struct inode{
  uint16_t i_mode; //specify the file format, and access right
  unsigned short i_uid; //specify the user id associated with the file
  unsigned int i_size; //it indicates the size of the file in readBytes
  uint32_t i_atime; //it gives total seconds between janurary 1st 1970 and last time this inode was accessed
  uint32_t i_ctime; //it gives total seconds between jaurary 1st 1970 and last time this inode was created
  uint32_t i_mtime; //between janurary 1st 1970 and last modified time
  uint32_t i_dtime; // last time it was deleted
  uint16_t i_gid; //Block group id associated with the inode
  uint16_t i_links_count; //it is the number of time this inode is linked
  uint32_t i_blocks; //it is the total number of 512 bytes- block which contain the data of this inode
  uint32_t i_flags; //it tells how ext2 implementation should behave while acessing the inode data
  uint32_t i_osd1;  //Os dependent value
  uint32_t i_block[15];  // Array of pointers to first 12 blocks, first indirect block, doubly indirect block, and tribly indirect blockSize
  uint32_t i_generation;  //Version of file
  uint32_t i_file_acl;  // Block id containg the extended attributes
  uint32_t i_dir_acl; // In revison 1, it store the value of upper 32 bit of the regular file size of 64 bits.
  uint32_t i_faddr;   //32 bits value indicating the value of the file Fragments
  uint16_t i_osd2 [6]; //
};

#endif
