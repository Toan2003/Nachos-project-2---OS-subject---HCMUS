#include "syscall.h" 
#include "copyright.h" 
#define maxlen 32 
int 
main() 
{ 
   int id;
   Create("test.txt");
   id=Open("test.txt", 1);
   Create("testt.txt");
   id=Open("testt.txt",1);
   PrintInt(id);
   Halt(); 
}