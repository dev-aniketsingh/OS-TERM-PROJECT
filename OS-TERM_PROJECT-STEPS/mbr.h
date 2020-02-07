#ifndef MBR
#define MBR
#include"partitionData.h"

const unsigned short BOOT_SECTOR_MAGIC = 0xaa55;

struct mbrSector{
  unsigned char bootStrapCode[446];
  struct partitionEntry partitionEntryInfo[4];
  uint16_t bootSignature;
};

#endif
