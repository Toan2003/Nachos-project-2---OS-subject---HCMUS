#include "syscall.h" 
#include "copyright.h" 
#define maxlen 32 
int main() 
{ 
   int id,sizefile1;
   char buffer[900];
   char filename[maxlen];
   PrintString("Enter File's name you would like to read: ");
   ReadString(filename,20);
   id=Open(filename, 1);
   if(id>0&& id<21)
   {
        sizefile1=Seek(-1,id);
        Seek(0,id);
        Read(buffer,sizefile1,id);
        PrintString("File's content: ");
        PrintString(buffer);
        //PrintString("\n");
   }
   Close(id);
   Halt(); 
}