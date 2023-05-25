#include "syscall.h"

int main() {
    int a,c;
    char* b[2];
    char d[]= "testExecV";
    char e[]= "Emne";
    a=2;
    // char f[]="test";
    // char g[]="tests";
    // char k[]="ssss";
    // a = 2;
    // // PrintString("hello");
    b[0] = d;
    b[1] = e;
    // b[2] = f;
    // b[3] = g;
    // b[4] = k;
    // PrintString(b[0]);
    // PrintString(b[1]);
    // PrintString(b[2]);
    //  PrintString(b[3]);
    //   PrintString(b[4]);
    // // PrintString("hello");
    c = ExecV(a,b);
    while(1){};
}