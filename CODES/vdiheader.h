#ifndef VDIHEADER
#define VDIHEADER
#pragma pack(1)
struct vdiheader{
  uint8_t imageName[64];
  uint32_t signature;
  uint32_t version;
  uint32_t headerSize;
  uint32_t imageType;
  uint32_t flag;
  uint8_t translationMap[256];
  uint32_t mapOffset;
  uint32_t frameOffset;   //offsetData
  uint32_t logicalChs;
  uint32_t sectorSize1;
  uint32_t virtualChs;
  uint32_t sectorSize;
  uint32_t frameAllocated1;
  uint32_t diskSize;
  uint32_t test;
  uint32_t frameSize;
  uint32_t extraFrameSize;
  uint32_t totalFrame;
  uint32_t frameAllocated;
};
#endif
