#include "syscall.h" 
#define maxlen 32
int main() 
{ 
   int id,sizefile1;
   char filename[maxlen];
   PrintString("Enter File name: ");
   ReadString(filename,20);
   if(Create(filename)==0)
   {
      PrintString("Success create file.");
   }
   else
      PrintString("Create file failed!");
   Halt(); 
}