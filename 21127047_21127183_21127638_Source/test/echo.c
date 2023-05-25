#include "syscall.h"
#define maxLength 32

int main()
{
    char *buffer;
    int receivesize;
    int socket2, socket3, socket4;
    int socket1 = SocketTCP();
    socket2 = SocketTCP();
    socket3 = SocketTCP();
    socket4 = SocketTCP();

    if (socket1 != -1)
        PrintString("\nsuccess1");
    else
        PrintString("\nfail1");

    if (Connect(socket1, "127.0.0.1", 49000) == 0)
        PrintString("\nsuccess2");
    else
        PrintString("\nfail2");

    if (Send("i'm socet 1", 25, socket1) > 0)
        PrintString("\nsuccess3");
    else
        PrintString("\nfail3");

    receivesize = Receive(buffer, 25, socket1);
    if (receivesize > 0)
    {
        PrintString("\nsuccess4");
    }
    else
        PrintString("\nfail4");
    PrintString(buffer);

    if (socket2 != -1)
        PrintString("\nsuccess1");
    else
        PrintString("\nfail1");

    if (Connect(socket2, "127.0.0.1", 49000) == 0)
        PrintString("\nsuccess2");
    else
        PrintString("\nfail2");

    if (Send("i'm socet 2", 25, socket2) > 0)
        PrintString("\nsuccess3");
    else
        PrintString("\nfail3");

    receivesize = Receive(buffer, 25, socket2);
    if (receivesize > 0)
    {
        PrintString("\nsuccess4");
    }
    else
        PrintString("\nfail4");
    PrintString(buffer);

    if (socket3 != -1)
        PrintString("\nsuccess1");
    else
        PrintString("\nfail1");

    if (Connect(socket3, "127.0.0.1", 49000) == 0)
        PrintString("\nsuccess2");
    else
        PrintString("\nfail2");

    if (Send("i'm socet 3", 25, socket3) > 0)
        PrintString("\nsuccess3");
    else
        PrintString("\nfail3");

    receivesize = Receive(buffer, 25, socket3);
    if (receivesize > 0)
    {
        PrintString("\nsuccess4");
    }
    else
        PrintString("\nfail4");
    PrintString(buffer);

    if (socket4 != -1)
        PrintString("\nsuccess1");
    else
        PrintString("\nfail1");

    if (Connect(socket4, "127.0.0.1", 49000) == 0)
        PrintString("\nsuccess2");
    else
        PrintString("\nfail2");

    if (Send("i'm socet 4", 25, socket4) > 0)
        PrintString("\nsuccess3");
    else
        PrintString("\nfail3");

    receivesize = Receive(buffer, 25, socket4);
    if (receivesize > 0)
    {
        PrintString("\nsuccess4");
    }
    else
        PrintString("\nfail4");
    PrintString(buffer);
    Close(socket1);
    Close(socket2);
    Close(socket3);
    Close(socket4);
    Exit(0);
}