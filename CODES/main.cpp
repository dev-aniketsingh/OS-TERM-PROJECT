#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include <fstream>
#include<iostream>
#include<iomanip>
#include<bitset>
#include<cmath>
#include<stack>
#include"vdiheader.h"
#include"vdifile.h"
#include"vdifunctions.h"
#include"partitionData.h"
#include"mbr.h"
#include<time.h>
#include"partitionFunctions.h"
#include"superblock.h"
#include"superblockFunctions.h"
#include"blockGroupDescriptor.h"
#include"ext2File.h"
#include"inodeFunctions.h"
#include"directoryEntry.h"
#include"inode.h"
#include<cstring>
#include<vector>
#include<sstream>
#include<algorithm>


/*
This website has been useds quite often to understand the ext2 structure
https://www.nongnu.org/ext2-doc/ext2.html
*/

using namespace std;
int fetchDirectoryEntry(struct Entry &,unsigned char[],string,struct inode,int,int);
void displayTranslationMap(struct vdifile * file);
void displayUUID(struct vdifile*,struct UUID*);
void displayInodeMeta(unsigned char inodeMetaData[],int iNum);
bool Contains(string s1, string s2);
void ClearScreen();
struct __attribute__((packed)) UUID {
    uint32_t
        timeLow;
    uint16_t
        timeMid,
        timeHigh,
        clock;
    uint8_t
        node[6];
};
static char *uuid2ascii(struct UUID *);
static char
    uuidStr[40];

int main(int argc, char* argv[]){
  struct vdifile * file ;
  if(argc < 2) {
    cout <<"File not included \n";
    return 1;
  }
  int mbrReadBytes;
  struct mbrSector mbrData;
  struct dataBlock data;
  file = vdiOpen(argv[1]);
 /*
  it reads the vdi header
 */
  vdiRead(file,&(file->header),sizeof(file->header));
  struct UUID id;
  dumpVDIHeader(file);
  displayUUID(file,&id);
  displayTranslationMap(file);

   /* Change the position of file descriptor pointer and read the mbr data*/
   mbrSeek(file,file->header.frameOffset,SEEK_SET);
   mbrReadBytes=mbrRead(file,mbrData,sizeof(mbrData));
   displayPartitionInfo(mbrData);
   int offsetToSuperBlock= file->header.frameOffset+mbrData.partitionEntryInfo[0].logicalBlocking*512+1024;
   vdiSeek(file,offsetToSuperBlock,SEEK_SET);
   vdiRead(file,&data,sizeof(data));
   displaySuperBlock(file,data,mbrData);
   /*
     It reads the translation map , which can be used to translate the virtual page to physical page in dynamic vdi file
   */
   int offsetToTranslationMap,
       readBytesOfTranslationMap,
       count=0;
   int translationMapData[4*file->header.totalFrame];
   offsetToTranslationMap=vdiSeek(file,file->header.mapOffset,SEEK_SET);
   readBytesOfTranslationMap= vdiRead(file,translationMapData,sizeof(translationMapData));
   /*
    This reads the super block in located in the partittion
   */
  struct ext2File ex;
  struct ext2File * ext2 = &ex;
  readSuperBlock(ext2,0,file,mbrData,translationMapData);
  displaySuperBlock(ext2);
  /*
    This given line of codes can be used to read the block group descriptor table data
  */
  int blockSize= 1024<<(ext2->superblock.s_log_block_size);
  int totalBlockGroup= ((ext2->superblock).s_blocks_count-ext2->superblock.s_first_data_block)/(ext2->superblock).s_blocks_per_group;
  if(((ext2->superblock).s_blocks_count-ext2->superblock.s_first_data_block)%(ext2->superblock).s_blocks_per_group>0){
     totalBlockGroup++;
  }
  int offsetToBlockGroup;
  struct blockGroupDescriptor table[totalBlockGroup];
  if(blockSize==4096){
    offsetToBlockGroup= offsetToSuperBlock-1024+blockSize;
    vdiSeek(file,offsetToBlockGroup,SEEK_SET);
  }
  else{
    offsetToBlockGroup= offsetToSuperBlock+blockSize;
    vdiSeek(file,offsetToBlockGroup,SEEK_SET);
  }
  vdiRead(file,table,sizeof(table));
  cout<<"Block Group NO.\t"<<"Block Bit Map"<<"\t"<<"Inode Bitmap"<<"\t"<<"Innode Table"<<"\t"<<"Free Blocks"<<"\t"<<"Free Inodes"<<"\t"<<"Used Directory"<<"\n";
/*.
  it can be used to display the content of the group descriptor of the registered block group
*/
  for(int i=0;i<totalBlockGroup;i++){
    cout<<i<<"\t\t";
    cout<<table[i].bg_block_bitmap<<"\t\t";
    cout<<table[i].bg_inode_bitmap<<"\t\t";
    cout<<table[i].bg_inode_table<<"\t\t";
    cout<<table[i].bg_free_blocks<<"\t\t";
    cout<<table[i].bg_free_inodes<<"\t\t";
    cout<<table[i].bg_dirs_counts<<"\t\t";
    cout<<endl;
  }
  /*
    This given code is used to read the given inode
  */
  struct inode  in;
  int readInodeBytes;
  unsigned char inodeMetaData[128];
  fetchInode(ext2,file,table,12,in,offsetToSuperBlock,translationMapData,inodeMetaData);
  displayInodeMeta(inodeMetaData,12);
  displayInode(in);
  fetchInode(ext2,file,table,2,in,offsetToSuperBlock,translationMapData,inodeMetaData);
  displayInodeMeta(inodeMetaData,2);
  displayInode(in);
  /*
  This given lines code can be used to fetch the data block inside the file
  */
  unsigned char buff[blockSize];
  int isFetched,totalBlocksInFile,
      remainingSpace,size;
  struct Entry currentDirectory, directory;
  stack<struct Entry>* directories= new stack<struct Entry>();
  int rootDirectorySize= (in.i_size)/blockSize;
  if(in.i_size%blockSize !=0){
    rootDirectorySize++;
  }
  for(int i=0;i<rootDirectorySize;i++){
    fetchBlockFromFile(&in,i,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
    isFetched= fetchDirectoryEntry(currentDirectory,buff,".",in,blockSize,i);
    directory = currentDirectory;
    directories->push(currentDirectory);
    if(isFetched==0){
      break;
    }
  }
  if(isFetched ==1){
    cout<<"Unable to open the directory file "<<endl;
  }
  bool run= true,isIt= false;
  string ext2Path="",command;
  cout << "*************************************************************************************************************************************" << "\n";
  cout << "*************************************************************************************************************************************" << "\n";
  cout << "*************************************************************************************************************************************" << "\n";
  cout << "*************************************************************************************************************************************" << "\n";

  cout << "Below commands are provided to navigate through different directories in ext2 file system " << "\n";
  cout << "The commands are : " << "\n" ;
  cout << "List all items: ls " << "\n";
  cout << "Long list:  ls -l" << "\n";
  cout << "To terminate the project: quit " << "\n";
  cout << "Change Directory: cd [Directory Name]" << "\n";
  cout << "Go back to previous directory: cd" << "\n";
  cout << "To clear screen: clear" << "\n";
  cout << "*******************Read Instructions*********************" << "\n";
  cout << "To read from vdi file to your host computer: " << "\n";
  cout << " read path_in_the_vdi_file (eg: /examples/08.Strings) _host_path_where_the_file_must_be_copied_ (eg: /home/nirajan/Documents/test.cpp)" << "\n";
  cout << "*******************Write Instructions*********************" << "\n";
  cout << "To read from vdi file to your host computer: " << "\n";
  cout << " write _path__in_the_vdi_file_must_be_copied_into_       _host_path_where_the_file_must_be_copied_from" << "\n";
  cout << endl;
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";

/*  int test[blockSize/4];
  fetchBlock(ext2,1434,file,mbrData,translationMapData,blockSize/4,test);
  cout<<endl<<endl;
  int count1 =0;
  for(int x: test){
    count1++;
    cout<<x<<"\t";
    if(count==16){
      count1=0;
      cout<<endl;
    }
  }*/
 while(run){
   cout << "Type your command here: " << "\n";
    cout<<"/"<<ext2Path;
    cout << endl;
    getline(cin,command);
//  cout << " value"  <<  command.compare(0,3,"read ") << endl;
    if(command=="ls"){
      struct Entry * tempDirectory= (struct Entry * ) malloc(sizeof(struct Entry));
      if(fetchInode(ext2,file,table,currentDirectory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData)){
        totalBlocksInFile= in.i_size/blockSize;
        if(in.i_size%blockSize !=0) totalBlocksInFile++;
        for(int j=0;j<totalBlocksInFile;j++){
          isIt= fetchBlockFromFile(&in,j,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
          size=0;
          remainingSpace= in.i_size -j*blockSize;
          if(remainingSpace>=blockSize) remainingSpace= blockSize;
          while(size<remainingSpace){
              memcpy(tempDirectory,buff+size,sizeof(struct Entry));
              char fileName[tempDirectory->nameLength+1];
              memcpy(fileName,tempDirectory->name,tempDirectory->nameLength);
              fileName[tempDirectory->nameLength]='\0';
              cout<<fileName<<"\n";
              size+= tempDirectory->recordLength;
          }
       }
      }
      free(tempDirectory);
    }
    else if(command == "clear") {
      ClearScreen();
    }
    else if(command == "ls -l") {
        /*
        Some of the websites used to understand long list structure:
        https://linuxconfig.org/understanding-of-ls-command-with-a-long-listing-format-output-with-permission-bits
        https://www.techrepublic.com/blog/linux-and-open-source/get-more-out-of-the-ls-command/
        */
      struct Entry entry;
      struct inode tempInode;
      fetchInode(ext2,file,table,currentDirectory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
      totalBlocksInFile = in.i_size/blockSize;
      if(in.i_size % blockSize != 0) totalBlocksInFile++;
      //cout<<"Total : "<<totalBlocksInFile<<"\n";
      for(int i =0; i < totalBlocksInFile; i++) {
        fetchBlockFromFile(&in,i,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
        size = 0;
      //  cout<<"Inode Size: "<<in.i_size<<"\n";
        remainingSpace = in.i_size - i * blockSize;
        if(remainingSpace >= blockSize) remainingSpace = blockSize;
      //  cout<<"remainingSpace: "<<remainingSpace<<"\n";
        while(size < remainingSpace) {
          memcpy(&entry,buff+size, sizeof(struct Entry));
          char name[entry.nameLength+1];
          memcpy(name, entry.name,entry.nameLength);
          name[entry.nameLength] = '\0';
          isFetched = fetchDirectoryEntry(entry,buff,(string)name,in,blockSize,i);
          if(isFetched == 0) {
            fetchInode(ext2,file,table,entry.inodeNumber,tempInode,offsetToSuperBlock,translationMapData,inodeMetaData);
            int sizeOfFile=tempInode.i_size;
            //permissions
            cout << std::left <<setw(35) << entry.name;
            /*
             This website helped for the part below
             http://cs.smith.edu/~nhowe/262/oldlabs/ext2.html
            */
            if(S_ISREG(tempInode.i_mode))   cout << "-";
            if(S_ISDIR(tempInode.i_mode))   cout << "d";
            if(S_ISCHR(tempInode.i_mode))   cout << "c";
            if(S_ISBLK(tempInode.i_mode))   cout << "b";
            if(S_ISFIFO(tempInode.i_mode))  cout << "p";
            if(S_ISSOCK(tempInode.i_mode))  cout << "s";
            if(S_ISLNK(tempInode.i_mode))   cout << "l";
            if (tempInode.i_mode & S_IRUSR) cout<<"r"; else cout << "-";
            if (tempInode.i_mode & S_IWUSR) cout<<"w"; else cout << "-";
            if (tempInode.i_mode & S_IXUSR) cout<<"x"; else cout << "-";
            if (tempInode.i_mode & S_IRGRP) cout<<"r"; else cout << "-";
            if (tempInode.i_mode & S_IWGRP) cout<<"w"; else cout << "-";
            if (tempInode.i_mode & S_IXGRP) cout<<"x"; else cout << "-";
            if (tempInode.i_mode & S_IROTH) cout<<"r"; else cout << "-";
            if (tempInode.i_mode & S_IWOTH) cout<<"w"; else cout << "-";
            if (tempInode.i_mode & S_IXOTH) cout<<"x"; else cout << "-";
            cout << right << std::setw(15);
          //  cout << S_ISDIR(in.i_mode) << endl;
          /*
           http://www.cplusplus.com/reference/ctime/localtime/
          */

            time_t rawtime  = (const time_t) tempInode.i_mtime;
            struct tm * timeinfo;
            timeinfo = localtime (&rawtime);


            cout << tempInode.i_uid;
            cout << right <<setw(16);
            cout << tempInode.i_gid;
            cout << right <<setw(16);
            cout << std::dec << (sizeOfFile)  << " bytes      ";
            cout << right << setw(16);
            cout << asctime(timeinfo);
            cout << right;
            cout << endl;
          }
          size += entry.recordLength;
        }
      }
    }
    else if(command =="cd"){
      int length= directories->top().nameLength;
      struct Entry last= directories->top();
      directories->pop();
      if(directories->empty()){
        cout<<"You are at the root directory"<<"\n";
        directories->push(last);
        currentDirectory= directories->top();
      }
      else{
        currentDirectory= directories->top();
        ext2Path= ext2Path.substr(0,ext2Path.length()-length-1);
      }
    }
    else if(command.length()>3 && Contains(command, "cd ")){
      string destDirectory= command.substr(3,command.length()-3);
      currentDirectory= directories->top();
        fetchInode(ext2,file,table,currentDirectory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
        totalBlocksInFile= in.i_size/ blockSize;
        if(in.i_size% blockSize !=0) totalBlocksInFile++;
        for(int k=0;k<totalBlocksInFile;k++){
          isIt= fetchBlockFromFile(&in,k,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
          isFetched = fetchDirectoryEntry(currentDirectory,buff,destDirectory,in,blockSize,k);
          if(isFetched ==0){
            if((int)currentDirectory.file_type ==2){
              directories->push(currentDirectory);
              ext2Path +=(destDirectory+"/");
              cout<<"Entered"<<"\n";
              break;
            }
            else{
              cout<<"It isn't the directory file. So you can't change the diretory"<<endl;
              currentDirectory= directories->top();
            }
          }
          else{
            cout<<"Unable to fetch the destined directory"<<endl;
          }
        }
    }
    else  if(command.length() > 4 && Contains(command, "read ") ) {
      string newString = command.substr(6, command.length() - 6);
      stringstream sStream(newString);
      struct Entry tempDirectory = directory;
      vector<string> path, diretoryName;
      string temp;
      while(getline(sStream, temp, ' ')) {
        path.push_back(temp);
      }

      stringstream ss(path[0]);
      while(getline(ss,temp, '/')){
        diretoryName.push_back(temp);
      }
      if(diretoryName.size()==0)diretoryName.push_back("..");
      path[1] += ("/" + diretoryName[diretoryName.size()-1]);
      char * hostpath= (char * )malloc(path[1].length());
      for(int i=0;i<path[1].length();i++){
        *(hostpath+i)= path[1][i];
      }
      cout<<hostpath<<endl;
      int op = open(hostpath, O_CREAT| O_RDWR|O_TRUNC, S_IRWXU );
      free(hostpath);
      for(int j = 0; j < diretoryName.size(); j++) {
          fetchInode(ext2,file,table,tempDirectory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
          int numBlock = in.i_size/blockSize;
          if(in.i_size % blockSize != 0 ) numBlock++;
          for (int i = 0; i < numBlock; i++)  {
            isIt = fetchBlockFromFile(&in,i,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
            isFetched = fetchDirectoryEntry(tempDirectory,buff,diretoryName[j],in,blockSize,i);
            if(isFetched == 0)
              break;
          }

      }
      fetchInode(ext2,file,table,tempDirectory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
      totalBlocksInFile = in.i_size/blockSize;
      if(in.i_size % blockSize != 0 ) totalBlocksInFile++;
      lseek(op,0,SEEK_SET);
      for (int i = 0; i < totalBlocksInFile-1; i++) {
        isIt= fetchBlockFromFile(&in,i,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
        //cout<<"is it "<<isIt<<"\n";
        if(write(op, buff, sizeof(buff)) == -1) {
          cout << "Unable to write" << "\n";
        }
      }
      int left=0;
      if(in.i_size%blockSize !=0) left= in.i_size %blockSize;
      else left= blockSize;
      fetchBlockFromFile(&in,totalBlocksInFile-1,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
      if(write(op, buff,left) == -1) {
        cout << "Unable to write" << "\n";
      }

   }
   else if(command == "quit"){
     return EXIT_FAILURE;
   }
   else if(command.length() > 5 && Contains(command, "write ")) {
     string newString = command.substr(7, command.length() - 7);
     stringstream sStream(newString);
     vector<string> path, diretoryName;
     struct Entry tempDirectory = directory;
     string temp;
     while(getline(sStream, temp, ' ')) {
       path.push_back(temp);
     }
    stringstream ss(path[0]);
    while(getline(ss,temp, '/')) {
       diretoryName.push_back(temp);
    }
    if(diretoryName.size()==0)diretoryName.push_back("..");
    vector<string>userPath;
    stringstream s2(path[1]);
    while(getline(s2,temp,'/')){
      userPath.push_back(temp);
    }
    int fd = open(path[1].c_str(), O_RDWR|O_APPEND, S_IRWXU);
    if(fd == -1) {
      cout << "file could not open, check you path again" << "\n";
    }
    else{
      int fileSize = lseek(fd, 0, SEEK_END);
      if(fileSize > (ext2->superblock.s_free_blocks_count*blockSize)) cout<<"Sorry!! Vdi file doesn't have enough free blocks to hold the size of the given file"<<"\n";
      else{
       for(int j = 0; j < diretoryName.size(); j++) {
        fetchInode(ext2,file,table,tempDirectory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
        int numBlock = in.i_size/blockSize;
        if(in.i_size % blockSize != 0 ) numBlock++;
        for (int i = 0; i < numBlock; i++)  {
          isIt = fetchBlockFromFile(&in,i,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
          isFetched = fetchDirectoryEntry(tempDirectory,buff,diretoryName[j],in,blockSize,i);
          if(isFetched == 0)
            break;
        }
       }
       struct inode Inode;
       if(fetchInode(ext2,file,table,tempDirectory.inodeNumber,Inode,offsetToSuperBlock,translationMapData,inodeMetaData)){
        int inodeNumber = 0;
        int offsetIn,offsetBl;
        unsigned char inBitMap[ext2->superblock.s_inodes_per_group/8];
        fetchInodeBitMap(ext2,file,table,0,offsetToSuperBlock,translationMapData,inBitMap,offsetIn);
        if(!inodeInUse(ext2, inBitMap, inodeNumber)){
         //cout<<std::bitset<8>(inBitMap[inodeNumber/8])<<" ";
         allocateInode(inodeNumber,inBitMap,table,ext2,0);
         //cout<<std::bitset<8>(inBitMap[inodeNumber/8])<<" ";
         //cout<<"Inode Number : "<<inodeNumber+1<<endl;
         if(fetchInode(ext2,file,table,inodeNumber+1,in,offsetToSuperBlock,translationMapData,inodeMetaData)){

          //cout<<"inode Number : "<<in.i_size<<"\n";
           in.i_mode= 0x8000|0x0100|0x0080|0x0040;
           int n= blockSize/4;
           //cout<<std::bitset<16>(in.i_mode)<<" ";
           in.i_size= fileSize;
           in.i_uid=0x3e8;
           in.i_gid= 0x3e8;
           in.i_links_count =1;
           int numBlocksNeeded= fileSize/blockSize;
           if(fileSize%blockSize !=0) numBlocksNeeded++;
           //cout<<numBlocksNeeded<<" total blocks "<<endl;
           unsigned char blockBitMap[ext2->superblock.s_blocks_per_group/8];
           int blockGNum=0;
           fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
           if(numBlocksNeeded<=12){
             for(int j=0;j<numBlocksNeeded;j++){
               in.i_block[j]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(in.i_block[j]==-1 &&blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                 j--;
               }
             }
             in.i_block[12]=0;
             in.i_block[13]=0;
             in.i_block[14]=0;
           }
           else if(numBlocksNeeded<=12+n){
             bool go= true;
             int blocksWithin=0;
             for(int d=0;d<12;d++){
               in.i_block[d]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(in.i_block[d] ==-1 && blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                 d--;
               }
             }
             while(go){
               in.i_block[12]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(in.i_block[12] ==-1 && blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
               }
               else go= false;
             }
             blocksWithin= numBlocksNeeded-12;
             int allocatedBlocks[blocksWithin];
             for(int i=0;i<blocksWithin;i++){
               allocatedBlocks[i]=allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(allocatedBlocks[i] ==-1 && blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
               }
             }
             writeBlock(ext2,in.i_block[12],file,mbrData,translationMapData,allocatedBlocks,sizeof(allocatedBlocks));
             in.i_block[13]=0;
             in.i_block[14]=0;
             //for(int x: in.i_block) cout<<"inode blocks : "<<x<<"\n";
             in.i_blocks = ((numBlocksNeeded*blockSize)/512)+blockSize/512;
           }
           else if(numBlocksNeeded<=12+n+pow(n,2)){
             bool keep= true;
             bool go = true;
             int blocksWithin=0;
             for(int d=0;d<12;d++){
               in.i_block[d]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(in.i_block[d] ==-1 && blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                 d--;
               }
             }
             /*
             Allocate the blocks for single indirect blocks
             */
             while(go){
               in.i_block[12]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(in.i_block[12] ==-1 && blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
               }
               else go= false;
             }

             blocksWithin=n;
             int allocatedBlocks[blocksWithin];
             for(int i=0;i<blocksWithin;i++){
              allocatedBlocks[i]=allocateBlock(ext2,table,blockBitMap,blockGNum);
              if(allocatedBlocks[i]==-1 && blockGNum<totalBlockGroup){
                blockGNum++;
                lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                i--;
              }
             }
             writeBlock(ext2,in.i_block[12],file,mbrData,translationMapData,allocatedBlocks,sizeof(allocatedBlocks));

             /*
             Allocate the blocks for double indirect blocks
             */
              while(keep){
                in.i_block[13]= allocateBlock(ext2,table,blockBitMap,blockGNum);
                if(in.i_block[13]==-1 && blockGNum<totalBlockGroup){
                  blockGNum++;
                  lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                  write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                  fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                }
                else keep= false;
             }
             blocksWithin= numBlocksNeeded-12-n;
             int numberOfSingleRequired= blocksWithin/n;
             if(blocksWithin%n !=0) numberOfSingleRequired++;
             int singleIndirectBlocks[numberOfSingleRequired];
             for(int i=0;i<numberOfSingleRequired;i++){
               singleIndirectBlocks[i]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(singleIndirectBlocks[i]==-1 &&blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                 i--;
               }
            }
            writeBlock(ext2,in.i_block[13],file,mbrData,translationMapData,singleIndirectBlocks,sizeof(singleIndirectBlocks));
            int directBlocks[n];
            for(int j=0;j<(numberOfSingleRequired-1);j++){
              for(int k=0;k<n;k++){
                directBlocks[k]= allocateBlock(ext2,table,blockBitMap,blockGNum);
                if(directBlocks[k] ==-1 && blockGNum<totalBlockGroup){
                  blockGNum++;
                  lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                  write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                  fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                  k--;
                }
              }
              writeBlock(ext2,singleIndirectBlocks[j],file,mbrData,translationMapData,directBlocks,sizeof(directBlocks));
            }
            int rem;
            if(blocksWithin%n ==0) rem=n;
            else rem=blocksWithin%n;
            int remDirect[rem];
            for(int q=0;q<rem;q++){
              remDirect[q]=allocateBlock(ext2,table,blockBitMap,blockGNum);
              if(remDirect[q] ==-1 && blockGNum<totalBlockGroup){
                blockGNum++;
                lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                q--;
              }
            }
            writeBlock(ext2,singleIndirectBlocks[numberOfSingleRequired-1],file,mbrData,translationMapData,remDirect,sizeof(remDirect));
            in.i_block[14]=0;
            in.i_blocks = ((numBlocksNeeded*blockSize)/512)+(blockSize/512)+(blockSize/512)+(numberOfSingleRequired*blockSize)/512;
          }
          /*
          Allocate the blocks for double indirect blocks
          */
          else if(numBlocksNeeded<=12+n+pow(n,2)+pow(n,3)){
            bool go= true;
            bool keep= true;
            bool cont= true;
            int blocksWithin=0;
            for(int d=0;d<12;d++){
              in.i_block[d]= allocateBlock(ext2,table,blockBitMap,blockGNum);
              if(in.i_block[d] ==-1 && blockGNum<totalBlockGroup){
                blockGNum++;
                lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                d--;
              }
            }
            /*
            Allocate the blocks for single  indirect blocks
            */
            while(go){
              in.i_block[12]= allocateBlock(ext2,table,blockBitMap,blockGNum);
              if(in.i_block[12] ==-1 && blockGNum<totalBlockGroup){
                blockGNum++;
                lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
              }
              else go= false;
            }
            blocksWithin= n;
            int allocatedBlocks[blocksWithin];
            for(int i=0;i<blocksWithin;i++) allocatedBlocks[i]=allocateBlock(ext2,table,blockBitMap,blockGNum);
            writeBlock(ext2,in.i_block[12],file,mbrData,translationMapData,allocatedBlocks,sizeof(allocatedBlocks));

            /*
            Allocate the blocks for double indirect blocks
            */
            while(cont){
               in.i_block[13]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(in.i_block[13]==-1 && blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
               }
               else cont= false;
            }
            blocksWithin= pow(n,2);
            int numberOfSingleRequired= blocksWithin/n;
            int singleBlocks[numberOfSingleRequired];
            for(int i=0;i<numberOfSingleRequired;i++){
              singleBlocks[i]= allocateBlock(ext2,table,blockBitMap,blockGNum);
              if(singleBlocks[i]==-1 &&blockGNum<totalBlockGroup){
                blockGNum++;
                lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                i--;
              }
           }
           writeBlock(ext2,in.i_block[13],file,mbrData,translationMapData,singleBlocks,sizeof(singleBlocks));
           int direct[n];
           for(int j=0;j<numberOfSingleRequired;j++){
             for(int k=0;k<n;k++){
               direct[k]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(direct[k] ==-1 && blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                 k--;
               }
             }
             writeBlock(ext2,singleBlocks[j],file,mbrData,translationMapData,direct,sizeof(direct));
           }
            /*
            Allocate the blocks for double indirect blocks
            */
            while(keep){
               in.i_block[14]= allocateBlock(ext2,table,blockBitMap,blockGNum);
               if(in.i_block[14]==-1 && blockGNum<totalBlockGroup){
                 blockGNum++;
                 lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                 write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                 fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
               }
               else keep= false;
            }
            blocksWithin= numBlocksNeeded-12-n-pow(n,2);
            int numDoubleIndirect= blocksWithin/pow(n,2);
            if(blocksWithin%(n*n) !=0) numDoubleIndirect++;
            int doubleIndirectBlocks[numDoubleIndirect];
            for(int i=0;i<numDoubleIndirect;i++){
              doubleIndirectBlocks[i]= allocateBlock(ext2,table,blockBitMap,blockGNum);
              if(doubleIndirectBlocks[i]==-1 &&blockGNum<totalBlockGroup){
                blockGNum++;
                lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                i--;
              }
           }
           writeBlock(ext2,in.i_block[14],file,mbrData,translationMapData,doubleIndirectBlocks,sizeof(doubleIndirectBlocks));
           int numberOfSingle= blocksWithin/n;
           if(blocksWithin%n !=0) numberOfSingle++;
           int singleIndirectBlocks[numberOfSingle];
           int directBlocks[n];
           for(int j=0;j<(numDoubleIndirect-1);j++){
             for(int k=0;k<n;k++){
                singleIndirectBlocks[k]= allocateBlock(ext2,table,blockBitMap,blockGNum);
                if(singleIndirectBlocks[k] ==-1 && blockGNum<totalBlockGroup){
                  blockGNum++;
                  lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                  write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                  fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                  k--;
                }
                for(int f=0;f<n;f++){
                  directBlocks[f]= allocateBlock(ext2,table,blockBitMap,blockGNum);
                  if(directBlocks[f] ==-1 && blockGNum<totalBlockGroup){
                    blockGNum++;
                    lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                    write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                    fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                    f--;
                  }
                }
                writeBlock(ext2,singleIndirectBlocks[k],file,mbrData,translationMapData,directBlocks,sizeof(directBlocks));
             }
             writeBlock(ext2,doubleIndirectBlocks[j],file,mbrData,translationMapData,singleIndirectBlocks,4*n);
           }
           int remk;
           if((blocksWithin)%(n*n) ==0) remk=n;
           else remk=(blocksWithin)%(n*n);
           int index=(remk)/n;
           if((remk)%n !=0) index++;
           int remSingle[index];
           for(int e=0;e<index;e++){
             remSingle[e]= allocateBlock(ext2,table,blockBitMap,blockGNum);
             if(remSingle[e] ==-1 && blockGNum<totalBlockGroup){
               blockGNum++;
               lseek(file->fileDescriptor,offsetBl,SEEK_SET);
               write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
               fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
               e--;
             }
             if(e<=(index-2)){
              for(int f=0;f<n;f++){
                directBlocks[f]= allocateBlock(ext2,table,blockBitMap,blockGNum);
                if(directBlocks[f] ==-1 && blockGNum<totalBlockGroup){
                  blockGNum++;
                  lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                  write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                  fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                  f--;
                }
              }
             writeBlock(ext2,remSingle[e],file,mbrData,translationMapData,directBlocks,sizeof(directBlocks));
            }
           }
           writeBlock(ext2,doubleIndirectBlocks[numDoubleIndirect-1],file,mbrData,translationMapData,remSingle,sizeof(remSingle));
           int rem;
           if((blocksWithin%(n*n))%n ==0) rem=n;
           else rem=(blocksWithin%(n*n))%n;
           int remDirect[rem];
           for(int r=0;r<rem;r++){
             remDirect[r]=allocateBlock(ext2,table,blockBitMap,blockGNum);
             if(remDirect[r] ==-1 && blockGNum<totalBlockGroup){
               blockGNum++;
               lseek(file->fileDescriptor,offsetBl,SEEK_SET);
               write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
               fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
               r--;
             }
           }
           writeBlock(ext2,remSingle[index-1],file,mbrData,translationMapData,remDirect,sizeof(remDirect));
           in.i_blocks = ((numBlocksNeeded*blockSize)/512)+(blockSize/512)+(blockSize/512)+(numberOfSingleRequired*blockSize)/512+(blockSize/512)+((numDoubleIndirect*blockSize)/512)+(numberOfSingle*blockSize)/512;
         }
          struct Entry newDirectory;
          struct Entry lastDirectory;
          newDirectory.inodeNumber= inodeNumber+1;
          newDirectory.nameLength=userPath[userPath.size()-1].length();
          memcpy(newDirectory.name,userPath[userPath.size()-1].c_str(),newDirectory.nameLength);
          newDirectory.file_type= 1;
          int sizeofNewDir= 8+newDirectory.nameLength;
          if(sizeofNewDir%4 !=0) sizeofNewDir += (4-sizeofNewDir%4);
          newDirectory.recordLength= sizeofNewDir;
          struct inode destinedInode;
          //cout<<"Size : "<<sizeofNewDir<<"\n";
          fetchInode(ext2,file,table,tempDirectory.inodeNumber,destinedInode,offsetToSuperBlock,translationMapData,inodeMetaData);
          int requiredBlocks= (destinedInode.i_size)/blockSize;
          if(destinedInode.i_size%blockSize !=0) requiredBlocks++;
          /*This is done if there is any left over for placing any new directory entry*/
          bool isTrue= true;
          int recordLengthFreeDirectory=0;
          for(int q=0;q<requiredBlocks;q++){
            recordLengthFreeDirectory=0;
            fetchBlockFromFile(&destinedInode,q,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
            remainingSpace= destinedInode.i_size-q*blockSize;
            if(remainingSpace>=blockSize)remainingSpace= blockSize;
            size=0;
            while(size<remainingSpace){
              memcpy(&lastDirectory,buff+size,sizeof(struct Entry));
              //cout<<"inode : "<<lastDirectory.inodeNumber<<"\n"<<"record : "<<lastDirectory.recordLength<<"\n";
              recordLengthFreeDirectory+=lastDirectory.recordLength;
              int newRecordLength= 8+lastDirectory.nameLength;
              if(newRecordLength%4 !=0) newRecordLength += (4- newRecordLength%4);
              if(recordLengthFreeDirectory==blockSize && (lastDirectory.recordLength-newRecordLength)>=sizeofNewDir){
                recordLengthFreeDirectory = recordLengthFreeDirectory-lastDirectory.recordLength;
                int temp= lastDirectory.recordLength;
                lastDirectory.recordLength =newRecordLength;
                temp = temp-lastDirectory.recordLength;
                newDirectory.recordLength = temp;
                memcpy(buff+recordLengthFreeDirectory,&lastDirectory,lastDirectory.recordLength);
                writeBlockToFile(&destinedInode,q,tempDirectory.inodeNumber,blockSize,ext2->superblock,ext2,file,mbrData,translationMapData,table,
                                   offsetToSuperBlock,buff,blockBitMap,blockGNum,sizeof(buff));
                memcpy(buff+(blockSize-temp),&newDirectory,sizeofNewDir);
                bool isWritten=writeBlockToFile(&destinedInode,q,tempDirectory.inodeNumber,blockSize,ext2->superblock,ext2,file,mbrData,translationMapData,table,
                                   offsetToSuperBlock,buff,blockBitMap,blockGNum,sizeof(buff));
                isTrue= false;
                break;
              }
                size += lastDirectory.recordLength;
            }
            if(!isTrue) break;
          }
          if(isTrue){
            if(requiredBlocks<=12){
              bool move=true;
              while(move){
                destinedInode.i_block[requiredBlocks]=allocateBlock(ext2,table,blockBitMap,blockGNum);
                if(destinedInode.i_block[requiredBlocks]==-1 && blockGNum<totalBlockGroup){
                  lseek(file->fileDescriptor,offsetBl,SEEK_SET);
                  write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap));
                  blockGNum++;
                  fetchBlockBitMap(ext2,file, table,blockGNum,offsetToSuperBlock,translationMapData,blockBitMap,offsetBl);
                }
                else move= false;
              }
            }
            /*
            I don't think that a directory file requires more than 12 blocks to keep directories entries
            Each directories entry occupy at max 263 bytes. So, (12*1024)/263 close to 436 directories entries.
            */
            unsigned char * writeBlock= (unsigned char *)malloc(sizeofNewDir);
            newDirectory.recordLength = blockSize;
            memcpy(writeBlock,&newDirectory,sizeofNewDir);
            if(!writeBlockToFile(&destinedInode,requiredBlocks,tempDirectory.inodeNumber,blockSize,ext2->superblock,ext2,file,mbrData,translationMapData,table,
                             offsetToSuperBlock,writeBlock,blockBitMap,blockGNum,sizeofNewDir)) cout<<"Unable to write "<<endl;
            destinedInode.i_size += blockSize;
            destinedInode.i_blocks += (blockSize/512);
            free(writeBlock);
          }
          /*
            This is used to write the file data to vdifile
          */
          unsigned char tempBuffer[blockSize];
          int blocksToWritten= (fileSize/blockSize);
          if(fileSize%blockSize) blocksToWritten++;
          for(int i=0;i<(blocksToWritten-1);i++){
            lseek(fd,i*blockSize,SEEK_SET);
            read(fd,tempBuffer,sizeof(tempBuffer));
            if(!writeBlockToFile(&in,i,inodeNumber+1,blockSize,ext2->superblock,ext2,file,mbrData,translationMapData,table,
                             offsetToSuperBlock,tempBuffer,blockBitMap,blockGNum,sizeof(tempBuffer))) cout<<"Unable to write data to file"<<"\n";
          }
          int leftPartInFile;
          if(fileSize %blockSize ==0 ) leftPartInFile= blockSize;
          else leftPartInFile= fileSize%blockSize;
          unsigned char remPart[leftPartInFile];
          read(fd,remPart,leftPartInFile);
          if(!writeBlockToFile(&in,blocksToWritten-1,inodeNumber+1,blockSize,ext2->superblock,ext2,file,mbrData,translationMapData,table,
                           offsetToSuperBlock,remPart,blockBitMap,blockGNum,leftPartInFile)) cout<<"Unable to write data to file"<<"\n";
          /*
            This is used to write the file data to vdifile
          */
          if(!writeInode(ext2,file,table,newDirectory.inodeNumber,&in,offsetToSuperBlock,translationMapData)) cout<<"unable to write"<<"\n";
          if(!writeInode(ext2,file,table,tempDirectory.inodeNumber,&destinedInode,offsetToSuperBlock,translationMapData)){
            cout<<"Unable to write inode to Vdi file "<<"\n";
          }
          lseek(file->fileDescriptor,offsetBl,SEEK_SET);
          if(write(file->fileDescriptor,blockBitMap,sizeof(blockBitMap))==-1) cout<<"Unable to write block Bit map"<<"\n";
          lseek(file->fileDescriptor,offsetIn,SEEK_SET);
          if(write(file->fileDescriptor,inBitMap,sizeof(inBitMap))==-1)cout<<"Unable to write inode Bit map "<<"\n";
          lseek(file->fileDescriptor,offsetToBlockGroup,SEEK_SET);
          if(write(file->fileDescriptor,table,sizeof(table))==-1)cout<<"Unable to write block group descriptor"<<"\n";
          //cout<<ext2->superblock.s_free_inodes_count<<"\n";
          writeSuperBlock(ext2,file,mbrData,ext2->superblock,translationMapData);
          readSuperBlock(ext2,0,file,mbrData,translationMapData);
          //cout<<ext2->superblock.s_free_inodes_count<<"\n";
          lseek(file->fileDescriptor,file->header.frameSize,SEEK_SET);
          if(write(file->fileDescriptor,translationMapData,4*file->header.totalFrame)==-1)cout<<"Unable to write translation map "<<endl;
          lseek(file->fileDescriptor,0,SEEK_SET);
          if(write(file->fileDescriptor,&file->header,sizeof(file->header))==-1)cout<<"Unable to write translation map "<<endl;
         }
       }
      }
     }
    }
   }
 }
  free(directories);
  return 0;
}

static char *uuid2ascii(struct UUID *uuid) {
    sprintf(uuidStr,"%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x",
            uuid->timeLow,uuid->timeMid,uuid->timeHigh,uuid->clock,
            uuid->node[0],uuid->node[1],uuid->node[2],uuid->node[3],
            uuid->node[5],uuid->node[5]);
    return uuidStr;
}
void displayTranslationMap(struct vdifile * file){
  int count=0;
  for(int z: file->header.translationMap ){
      cout<<hex<<z<<"\t";
      count++;
      if(count==16){
          cout<<endl;
          count=0;
      }
  }
}
void displayUUID(struct vdifile* file,struct UUID *id){
  vdiRead(file,id,sizeof(struct UUID));
  uuid2ascii(id);
  cout<<"UUID : "<<uuidStr<<"\n";
  vdiRead(file,id,sizeof(struct UUID));
  uuid2ascii(id);
  cout<<"Last Snap UUID: "<<uuidStr<<"\n";
  vdiRead(file,id,sizeof(struct UUID));
  uuid2ascii(id);
  cout<<"Link UUID :"<<uuidStr<<"\n";
  vdiRead(file,id,sizeof(struct UUID));
  uuid2ascii(id);
  cout<<"Parent UUID :"<<uuidStr<<"\n";
}
void displayInodeMeta(unsigned char * inodeMetaData,int iNum){
    int j=0;
    cout<<endl<<"---------------"<<"Inode :"<<iNum<<"--------"<<endl;

    for(int i=0;i<128;i++){
      cout<<std::hex<<(int)*(inodeMetaData+i)<<"\t";
      j++;
      if(j==16){
        cout<<endl;
        j=0;
      }
    }
    cout<<endl<<"---------------------------------------------------------------"<<endl;

}
int fetchDirectoryEntry(struct Entry & directory,unsigned char buff[],string fileName,struct inode in,int blockSize,int bNum){
  int remainingSpace= in.i_size-bNum*blockSize;
  if(remainingSpace>=blockSize){
    remainingSpace= blockSize;
  }
  int offset=0;
  while(offset<remainingSpace){
    memcpy(&directory,buff+offset,sizeof(struct Entry));
    char name[directory.nameLength+1];
    memcpy(name, directory.name,directory.nameLength);
    name[directory.nameLength]='\0';
    if((string)name== fileName){
      return 0;
    }
    offset += directory.recordLength;
  }
  return 1;
}

bool Contains(string s1, string s2) {
 int a = s1.find(s2);
   if(a == -1)
   return false;

 return true;
}
void ClearScreen() {
  int n;
  for (n = 0; n < 10; n++)
    printf( "\n\n\n\n\n\n\n\n\n\n" );
}
