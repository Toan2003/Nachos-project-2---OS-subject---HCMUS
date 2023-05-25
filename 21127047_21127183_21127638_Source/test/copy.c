#include "syscall.h"
#include "copyright.h" 
#define maxlen 32 
int main() 
{ 
   int id,id1,sizefile1;
   char buffer[900];
   char source[32];
   char dest[32];
   PrintString("Enter Source File name: ");
   ReadString(source,20);
   id=Open(source, 0);
    
   PrintString("Enter Destination File name: ");
   ReadString(dest,20);
   id1=Open(dest, 0);
    
   if((id>0&&id<20) && (id1>0&&id1<20))
   {
        sizefile1=Seek(-1,id);
        Seek(0,id);
        Read(buffer,sizefile1,id);
        Write(buffer,sizefile1,id1);
        Close(id);
        Close(id1);
   }
   else
   {
    PrintString("Can not open file!");
   }
   Halt(); 
}