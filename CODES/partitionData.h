#ifndef PARTITIONDATA
#define PARTITIONDATA
#pragma pack(1)
struct partitionEntry{
  unsigned char bootIndicater;
  unsigned char firstChs[3];
  unsigned char partitionDescriptor;
  uint8_t lastChs[3];
  uint32_t logicalBlocking;
  uint32_t numberOfSectorInPartition;
};
struct dataBlock{
  unsigned char firstData[256];
  unsigned char secondData[256];
  unsigned char thirdData[256];
  unsigned char fourthData[256];
};

#endif
