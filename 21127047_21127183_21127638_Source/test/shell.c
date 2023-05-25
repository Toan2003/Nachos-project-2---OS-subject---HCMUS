#include "syscall.h"

int main() {
    int a = Exec("ping");
    int b = Exec("pong");
    // Join(b);
    Join(a);
    Join(b);
    //Exit(0);
    Exit(12);

}