#include "syscall.h" 
#include "copyright.h" 
#define maxlen 32 
int 
main() 
{ 
   int id,sizefile1;
   char filename[32];
   PrintString("Enter File you would like to delete: ");
   ReadString(filename,20);
   id=Open(filename, 1);
   if(id!=-1)
   {
        Close(id);
        if(Remove(filename)!=0)
        {
            PrintString("\nDelete file failed!");
        }
        
   }
   else
        PrintString("\nDelete file failed!");
   Halt(); 
}