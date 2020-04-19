#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include <fstream>
#include<iostream>
#include<iomanip>
#include<bitset>
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
  struct blockGroupDescriptor table[totalBlockGroup];
  if(blockSize==4096){
    int offsetToBlockGroup= offsetToSuperBlock-1024+blockSize;
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
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";

  cout << "Instructions to navigate through the project " << "\n";
  cout << "All commands are case sensetive" << "\n";
  cout << "The commands that works are: " << "\n" ;
  cout << "List all items: ls " << "\n";
  cout << "Long list:  ls -l" << "\n";
  cout << "Change Directory: cd [Directory Name]" << "\n";
  cout << "Go back to previous directory: cd " << "\n";
  cout << "To clear screen: clear" << "\n";
  cout << "----------------------------Read Instructions---------------------------" << "\n";
  cout << "To read from vdi file to your host computer: " << "\n";
  cout << " read _path__in_the_vdi_file_       _host_path_where_the_file_must_be_copied_" << "\n";
  cout << endl;
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";
  cout << "=======================================================++++++++++++++================================================================" << "\n";


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
      struct Entry entry;
      fetchInode(ext2,file,table,currentDirectory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
      totalBlocksInFile = in.i_size/blockSize;
      if(in.i_size % blockSize != 0) totalBlocksInFile++;
      for(int i =0; i < totalBlocksInFile; i++) {
        fetchBlockFromFile(&in,i,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
        size = 0;
        remainingSpace = in.i_size - i * blockSize;
        if(remainingSpace >= blockSize) remainingSpace = blockSize;
        while(size < remainingSpace) {
          memcpy(&entry,buff+size, sizeof(struct Entry));
          char name[entry.nameLength+1];
          memcpy(name, entry.name,entry.nameLength);
          name[entry.nameLength] = '\0';
          isFetched = fetchDirectoryEntry(entry,buff,(string)name,in,blockSize,i);
          if(isFetched == 0) {
            fetchInode(ext2,file,table,entry.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
          if(in.i_size%blockSize!=0) in.i_size= (in.i_size/blockSize)*blockSize+blockSize;
          //permissions
            cout << std::left <<setw(35) << entry.name;

            if(S_ISREG(in.i_mode))   cout << "-";
            if(S_ISDIR(in.i_mode))   cout << "d";
            if(S_ISCHR(in.i_mode))   cout << "c";
            if(S_ISBLK(in.i_mode))   cout << "b";
            if(S_ISFIFO(in.i_mode))  cout << "p";
            if(S_ISSOCK(in.i_mode))  cout << "s";
            if(S_ISLNK(in.i_mode))   cout << "l";
            if (in.i_mode & S_IRUSR) cout<<"r"; else cout << "-";
            if (in.i_mode & S_IWUSR) cout<<"w"; else cout << "-";
            if (in.i_mode & S_IXUSR) cout<<"x"; else cout << "-";
            if (in.i_mode & S_IRGRP) cout<<"r"; else cout << "-";
            if (in.i_mode & S_IWGRP) cout<<"w"; else cout << "-";
            if (in.i_mode & S_IXGRP) cout<<"x"; else cout << "-";
            if (in.i_mode & S_IROTH) cout<<"r"; else cout << "-";
            if (in.i_mode & S_IWOTH) cout<<"w"; else cout << "-";
            if (in.i_mode & S_IXOTH) cout<<"x"; else cout << "-";
            cout << right << std::setw(15);
          //  cout << S_ISDIR(in.i_mode) << endl;

            time_t rawtime  = (const time_t) in.i_mtime;
            struct tm * timeinfo;
            timeinfo = localtime (&rawtime);


            cout << in.i_uid;
            cout << right <<setw(16);
            cout << in.i_gid;
            cout << right <<setw(16);
            cout << std::dec << (in.i_size)  << " bytes      ";
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
      directories->pop();
      if(directories->empty()){
        cout<<"You are at the root directory"<<"\n";
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
      vector<string> path, diretoryName;
      string temp;
      while(getline(sStream, temp, ' ')) {
        path.push_back(temp);
      }

      stringstream ss(path[0]);
      while(getline(ss,temp, '/')){
        diretoryName.push_back(temp);
      }
      path[1] += ("/" + diretoryName[diretoryName.size()-1]);
      char * hostpath= (char * )malloc(path[1].length());
      for(int i=0;i<path[1].length();i++){
        *(hostpath+i)= path[1][i];
      }
      cout<<hostpath<<endl;
      int op = open(hostpath, O_CREAT| O_WRONLY );
      free(hostpath);
      for(int j = 0; j < diretoryName.size(); j++) {
          fetchInode(ext2,file,table,directory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
          int numBlock = in.i_size/blockSize;
          if(in.i_size % blockSize != 0 ) numBlock++;
          for (int i = 0; i < numBlock; i++)  {
            isIt= fetchBlockFromFile(&in,i,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
            isFetched = fetchDirectoryEntry(directory,buff,diretoryName[j],in,blockSize,i);
            if(isFetched == 0)
              break;
          }
      }
      fetchInode(ext2,file,table,directory.inodeNumber,in,offsetToSuperBlock,translationMapData,inodeMetaData);
      totalBlocksInFile = in.i_size/blockSize;
      if(in.i_size % blockSize != 0 ) totalBlocksInFile++;
      for (int i = 0; i < totalBlocksInFile; i++) {
        isIt= fetchBlockFromFile(&in,i,ext2->superblock,ext2,file,mbrData,translationMapData,buff);
        if(write(op, buff, sizeof(buff)) == -1) {
          cout << "Unable to write" << "\n";
        }
      }
   }

   else if(command.length() > 5 && Contains(command, "write ")) {
     cout << "LOL" << endl;
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
