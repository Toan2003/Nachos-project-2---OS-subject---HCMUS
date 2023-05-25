#include "syscall.h" 
#include "copyright.h"
#define maxlen 32 
int main() 
{ 
   int id,id1,id2,sizefile1,sizefile2;
   int j=0;
   char buff1[500],buff2[500];
   char filename1[32];
   char filename2[32];
   char filename3[32];
   PrintString("Enter Source File 1 name: ");
   ReadString(filename1,32);
   PrintString("Enter Source File 2 name: ");
   ReadString(filename2,32);
   PrintString("Enter File Destination file name: ");
   ReadString(filename3,32);
    id=Open(filename1, 0);
    id1=Open(filename2, 0);
    id2=Open(filename3,0);
   if((id>0&&id<20) && (id1>0&&id1<20)&& (id2>0&&id2<20))
   {
      sizefile1= Seek(-1,id);
      sizefile2=Seek(-1,id1);
      
      Seek(0,id);
      Seek(0,id1);

      Read(buff1,sizefile1,id);
      Read(buff2,sizefile2,id1);
      Write(buff1,sizefile1,id2);
      Seek(sizefile1,id2);
      Write(buff2,sizefile2,id2);
   }
   Close(id);
   Close(id1);
   Close(id2);
   Halt(); 
}