#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "PCB.h"

#define MAX_PROCESS 10

class PTable {
private:
    PCB* pcb[MAX_PROCESS]; // mảng 10 con trỏ pcb
    int psize;
    Semaphore* bmsem;
public:
    PTable();
    ~PTable();
    int ExecUpdate(char* name);
    int ExecV(int argc,char**argv);
    void ExitUpdate(int ec);
    int JoinUpdate(int id);
    int GetFreeSlot();
    //bool IsExist(int pid);
    //oid Remove(int pid);
    char* GetFileName(int id);
    void Remove(int id);
};




#endif