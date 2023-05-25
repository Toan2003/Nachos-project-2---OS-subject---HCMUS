#include "syscall.h"
#define maxLength 32
int main ()
{
    char buffer[800];
    char filename[maxLength],filename1[maxLength];
    int receivesize;
    int id,id1,sizefile;
    int socket1=SocketTCP();
    if (socket1 != -1)
        PrintString("\nsuccess1");
    else
        PrintString("\nfail1");
    
    if (Connect(socket1,"127.0.0.1", 49000) == 0)
        PrintString("\nsuccess2");
    else
        PrintString("\nfail2");

    PrintString("\nEnter a file name: ");
    ReadString(filename,32);
    id=Open(filename,0);
    if(id>=0&& id<21)
   {
        sizefile=Seek(-1,id);
        Seek(0,id);
        Read(buffer,sizefile,id);  
   }

    if (Send(buffer, sizefile, socket1) > 0)
        PrintString("\nsuccess3");
    else
        PrintString("\nfail3");

    receivesize = Receive(buffer, sizefile, socket1);
    if (receivesize > 0)
    {
        PrintString("\nsuccess4");
        
    }
    else
        PrintString("\nfail4");
    PrintString("Enter Destination file: ");
    ReadString(filename1,32);
    id1=Open(filename1,0);
    Write(buffer,sizefile,id1);
    Close(id);
    Close(id1);
    Halt();
}