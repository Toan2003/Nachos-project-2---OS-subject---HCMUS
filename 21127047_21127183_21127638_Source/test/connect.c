#include "syscall.h"
#define maxLength 32


int main(){
    char* buffer;
    int receivesize;
    int socket = SocketTCP();
    if (socket != -1)
        PrintString("\nsuccess1");
    else
        PrintString("\nfail1");
    
    if (Connect(socket,"192.168.0.106", 49000) == 0)
        PrintString("\nsuccess2");
    else
        PrintString("\nfail2");

    if (Send("hello form the other side", 25, socket) > 0)
        PrintString("\nsuccess3");
    else
        PrintString("\nfail3");

    receivesize = Receive(buffer, 25, socket);
    if (receivesize > 0)
    {
        PrintString("\nsuccess4");
        
    }
    else
        PrintString("\nfail4");
    PrintString(buffer);

    Halt();
}