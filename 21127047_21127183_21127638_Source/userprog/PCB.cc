#include "PCB.h"
#include "synch.h"

PCB::PCB(int pid, char* fn) {
    this->processID = pid;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    mutex = new Semaphore("multex", 1);
    strcpy(this->filename, fn);
}
PCB::~PCB() {

    // if (filename != NULL) {
    //     delete filename;
    // }
    if (joinsem != NULL) delete joinsem;
    if (exitsem != NULL) delete exitsem;
    if (mutex != NULL) delete mutex; 
    joinsem = exitsem = mutex = NULL;
    if (thread != NULL) {
        if (kernel->currentThread->processID == thread->processID) {
            thread->FreeSpace();
            thread->Finish();
            thread = NULL;
        }
        //schedular will delete thread later
    }
}

void StartProcess_1(void* pid) {
    int id;
    id = *((int*)pid);
    //cout << "start process: " << id << endl;
    // Lay fileName cua process id nay
    char* fileName = kernel->pTab->GetFileName(id);
    AddrSpace* space;
    space = new AddrSpace(fileName);
    //space->Load(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }
    space->Execute();
    kernel->currentThread->space = space;

    // space->InitRegisters();	// set the initial register values
    // space->RestoreState();	// load page table register

    // kernel->machine->Run();	// jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

void StartProcess_2(void* argv) {
    char** arg= (char**)(argv);
    //cout << "start process: " << id << endl;
    // Lay fileName cua process id nay
    char* fileName = arg[0];
    AddrSpace* space;
    space = new AddrSpace(fileName);
    //space->Load(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }
    space->Execute();
    // kernel->currentThread->space = space;

    // space->InitRegisters();	// set the initial register values
    // space->RestoreState();	// load page table register

    // kernel->machine->Run();	// jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

int PCB::Exec(char* filename) {
    // cerr << filename << ' ' << pid << endl;
    mutex->P();
    //cout << filename << " " << processID <<  endl;
    this->thread = new Thread(filename);
    //cout <<"thread: " << processID << endl;
    this->thread->processID = processID;
    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        mutex->V();  // Nha CPU de nhuong CPU cho tien trinh khac
        return -1;    // Tra ve -1 neu that bai
    }
    // Gọi thực thi Fork(StartProcess_2,id) => Ta cast thread thành kiểu int,
    // sau đó khi xử ký hàm StartProcess ta cast Thread về đúng kiểu của nó.
    // Không được sử dụng biến id ở đây, vì biến id là biến cục bộ,
    // nên khi hàm này kết thúc thì giá trị của biến này cũng bị xóa
    // Đừng hỏi tôi đã mất bao lâu để nhận ra điều này :)
    this->thread->Fork(StartProcess_1, &(this->processID));

    mutex->V();
    // Trả về id.
    return this->processID;
}


int PCB::ExecV(int argc, char** argv) {
    // cerr << filename << ' ' << pid << endl;
    mutex->P();
    //cout << filename << " " << processID <<  endl;
    this->thread = new Thread(argv[0]);
    //cout <<"thread: " << processID << endl;
    this->thread->processID = processID;
    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        mutex->V();  // Nha CPU de nhuong CPU cho tien trinh khac
        return -1;    // Tra ve -1 neu that bai
    }
    // Gọi thực thi Fork(StartProcess_2,id) => Ta cast thread thành kiểu int,
    // sau đó khi xử ký hàm StartProcess ta cast Thread về đúng kiểu của nó.
    // Không được sử dụng biến id ở đây, vì biến id là biến cục bộ,
    // nên khi hàm này kết thúc thì giá trị của biến này cũng bị xóa
    // Đừng hỏi tôi đã mất bao lâu để nhận ra điều này :)
    this->thread->Fork(StartProcess_2, argv);
    mutex->V();
    // Trả về id.
    return this->processID;
}

char* PCB::GetFileName() {
    return filename;
}


void PCB::setThread(Thread* current) {
    this->thread = current;
}


void PCB::IncNumWait() {
    mutex->P();
    ++numwait;
    mutex->V();
}

void PCB::DecNumWait() {
    mutex->P();
    if (numwait > 0) --numwait;
    mutex->V();
}

void PCB::JoinWait() {
    // Gọi joinsem->P() để tiến trình chuyển sang trạng thái block và ngừng lại,
    // chờ JoinRelease để thực hiện tiếp.
    joinsem->P();
}

void PCB::ExitWait() {
    // Gọi exitsem-->V() để tiến trình chuyển sang trạng thái block và ngừng
    // lại, chờ ExitReleaseđể thực hiện tiếp.
    exitsem->P();
}

void PCB::JoinRelease() {
    // Gọi joinsem->V() để giải phóng tiến trình gọi JoinWait().
    joinsem->V();
}

void PCB::ExitRelease() {
    // Gọi exitsem->V() để giải phóng tiến trình đang chờ.
    exitsem->V();
}

void PCB::SetExitCode(int ec) { exitcode = ec; }

int PCB::GetExitCode() { return exitcode; }

int PCB::GetID() {return processID;}  