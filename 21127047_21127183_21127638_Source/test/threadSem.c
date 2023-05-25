#include "syscall.h"

int main() {
    int a = Signal("toan");
    if (a ==0) PrintString("Signal thanh cong");
    PrintString("da toi day");
    Exit(0);
}