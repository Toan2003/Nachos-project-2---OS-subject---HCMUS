#include "syscall.h"

int main() {
    int b;
    int a;
    a = CreateSemaphore("toan",0);
    if (a==0) {
        PrintString("CreateSemaphore thanh cong");
    }
    b = Exec("threadSem");
    a = Wait("toan");
    Join(b);
    if(a==0) {
        PrintString("Wait thanh cong");
    }

    Exit(0);
}