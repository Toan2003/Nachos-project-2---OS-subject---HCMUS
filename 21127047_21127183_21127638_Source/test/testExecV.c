#include "syscall.h"

int main(int argc, char **argv) {
    char* b = argv[0];
    PrintString("da nhay vao testExec");
   
    PrintString(b);
    // PrintString((char)(argc));
    PrintString(argv[1]);
}