#ifndef PARTITIONDATA
#define PARTITIONDATA

struct partitionEntry{
  unsigned char bootIndicater;
  uint8_t firstChs[3];
  uint8_t partitionDescriptor;
  uint8_t lastChs[3];
  uint32_t logicalBlocking;
  uint32_t numberOfSectorInPartition;
};
#endif
