// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include<iostream>
#define MaxFileLength 32
#define MAX_LENGTH_PROCESS_NAME 32
#define MAX_LENGTH_STRING 32
#define MAX_LENGTH_IP 46

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
void IncreasePC()
{
	/* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
}

char* User2System(int virtAddr, int limit)
{
	//cout <<"mode 1:" << " " << virtAddr<< endl;
	int i; //chi so index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; //can cho chuoi terminal
	if (kernelBuf == NULL)
		return kernelBuf;
		
	memset(kernelBuf, 0, limit + 1);
	
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		//cout << oneChar << " ";
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	//cout << endl;
	return kernelBuf;
}

char** User2System2(int virtAddr, int limit, int argc)
{	
	//cout << "mode doc 2: " <<endl;
	int i; //chi so index
	int j;
	int realAddr;
	int len;
	int oneChar;
	char** kernelBuf = NULL;
	kernelBuf = new char*[argc]; //can cho chuoi terminal
	realAddr = argc/2 + argc % 2;
	realAddr = realAddr*8;
	virtAddr = virtAddr + realAddr;
	cout << virtAddr << endl;
	for(i =0; i< argc;i++) {
		kernelBuf[i] = new char[limit+1];
		memset(kernelBuf[i], 0, limit + 1);
	}
	// if (kernelBuf == NULL)
	// 	return kernelBuf;
	len = 0;
	cout << endl;
	for(i =0; i< argc;i++) {
		for (j = 0; j < limit; j++)
		{
			kernel->machine->ReadMem(virtAddr+j, 1, &oneChar);
			// cout << virtAddr+j << " ";
			// cout << oneChar <<" ";
			// kernelBuf[i][j] = (char)oneChar;
			if (oneChar == 0){
				if ((j+1)%8 != 0) {
					virtAddr = virtAddr + ((j+1)/8 + 1)*8;
				} else {
					virtAddr = virtAddr + (j+1);
				}
				break;
			}
		}
	}
	//cout << endl;
	return kernelBuf;
}

int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
	case NoException:
	{
		kernel->interrupt->setStatus(SystemMode);
		DEBUG('a', "\n Switch to system mode.");
        break;
	}
	case PageFaultException:
	{
		DEBUG('a', "\n No valid translation found.");
		printf("\n\n No valid translation found.");
		kernel->interrupt->Halt();
		break;
	}
	case ReadOnlyException: {
		DEBUG('a', "\n Write attempted to page marked read-only");
		printf("\n\n Write attempted to page marked read-only");
		kernel->interrupt->Halt();
		break;
	}

	case BusErrorException: {
		DEBUG('a', "\n Translation resulted invalid physical address");
		printf("\n\n Translation resulted invalid physical address");
		kernel->interrupt->Halt();
		break;
	}

	case AddressErrorException: {
		DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		kernel->interrupt->Halt();
		break;
	}

	case OverflowException: {
		DEBUG('a', "\nInteger overflow in add or sub.");
		printf("\n\n Integer overflow in add or sub.");
		kernel->interrupt->Halt();
		break;
	}

	case IllegalInstrException: {
		DEBUG('a', "\n Unimplemented or reserved instr.");
		printf("\n\n Unimplemented or reserved instr.");
		kernel->interrupt->Halt();
		break;
	}

	case NumExceptionTypes: {
		DEBUG('a', "\n Number exception types");
		printf("\n\n Number exception types");
		kernel->interrupt->Halt();
		break;
	}
    case SyscallException:
	{
	switch(type) {
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;
		}
    	case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
			
			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
					/* int op2 */(int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);
			
			/* Modify return point */
			IncreasePC();
			return;
		
			ASSERTNOTREACHED();

			break;
		}
		case SC_Create:
		{
			int virtAddr;
			char *filename;
			DEBUG('a', "\n SC_CreateFile call ...");
			DEBUG('a', "\n Reading virtual address of filename");

			virtAddr = kernel->machine->ReadRegister(4); //Doc dia chi cua file tu thanh ghi R4
			DEBUG('a', "\n Reading filename.");

			filename = User2System(virtAddr, MaxFileLength + 1);
			if (strlen(filename) == 0)
			{
				printf("\n File name is not valid");
				DEBUG('a', "\n File name is not valid");
				kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
				IncreasePC();
				return;
				break;
			}
			if (filename == NULL)  //Neu khong doc duoc
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
				delete filename;
				IncreasePC();
				return;
				break;
			}
			DEBUG('a', "\n Finish reading filename.");
			
			if (!kernel->fileSystem->Create(filename)) //Tao file bang ham Create cua fileSystem, tra ve ket qua
			{
				//Tao file that bai
				printf("\n Error create file '%s'", filename);
				kernel->machine->WriteRegister(2, -1);
				delete filename;
				IncreasePC();
				return;
				
			}
			
			//Tao file thanh cong
			cout<<"Create file success"<<endl;
			kernel->machine->WriteRegister(2, 0);
			delete filename;
			IncreasePC(); //Day thanh ghi lui ve sau de tiep tuc ghi
			return;
			
			ASSERTNOTREACHED();
			break;
		}
		case SC_Open:
		{
			//Input: filename, type (0 for standard and 1 for read file)
			//Output: fileid or -1 for fail
			//case for fail: over 20 files - cannot find file name (not exist)

			int virtAddr=kernel->machine->ReadRegister(4);
			char *filename;
			int type=kernel->machine->ReadRegister(5);
			filename=User2System(virtAddr,MaxFileLength+1);

			//chec xem đã có đủ 20 file descriptor chưa
			// if(kernel->fileSystem->index>20)             //fail
			// {
			// 	printf("\n over 20 files are opened");
			// 	kernel->machine->WriteRegister(2,-1);
			// 	delete [] filename;
			// 	IncreasePC();
			// 	return;
			// 	break;
			// }
			int tempt=kernel->fileSystem->isFull();
			if(tempt==-1)
			{
				printf("\n over 20 files are opened");
				kernel->machine->WriteRegister(2,-1);
				delete [] filename;
				IncreasePC();
				return;
				break;
			}
			//file name hông nhận
			if (strlen(filename) == 0)
			{
				printf("\n File name is not valid");
				DEBUG('a', "\n File name is not valid");
				kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
				IncreasePC();
				return;
				break;
			}
			if (filename == NULL)  //Neu khong doc duoc
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
				delete [] filename;
				IncreasePC();
				return;
				break;
			}

			//success
			//only open file when type =1 or type =0
			if(type==1 || type==0)
			{
				if((kernel->fileSystem->openfile[tempt]=kernel->fileSystem->Open(filename,type))!=NULL)
				{
					cout<<"Success open file"<< " file id: " << tempt <<endl;
					kernel->fileSystem->filename[tempt]=filename;
					kernel->machine->WriteRegister(2,tempt); //index -1 vi after open thì nó đã cộng một vị trí index 
					// cout<<kernel->fileSystem->openfile[kernel->fileSystem->index-1]->type;
				}
				else
				{
					cout<<"Cannot open file"<<endl;
					kernel->machine->WriteRegister(2,-1);
				}
			}
			else
			{
				cout<<"Cannot open file because wrong type"<<endl;
				kernel->machine->WriteRegister(2,-1);
			}
			
			delete [] filename;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;

		}
		case SC_Close:
		{
			int id=kernel->machine->ReadRegister(4); 
			if(id <2 || id >=20 || kernel->fileSystem->openfile[id]==NULL)
			{
				cout<<"Invalid file id"<<endl;
				kernel->machine->WriteRegister(2,-1);
				IncreasePC();
				return;
				break;
			}
			
			kernel->machine->WriteRegister(2,0);
			kernel->fileSystem->filename[id]="";
			delete kernel->fileSystem->openfile[id];
			kernel->fileSystem->openfile[id]=NULL;
			cout<<kernel->fileSystem->filename[id]<<endl;
			cout<<"Close file success"<<endl;
			IncreasePC();
			return;
			break;
		}
		case SC_Read:
		{
			// Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
			// Output: -1: Loi, So byte read thuc su: Thanh cong, -2: Thanh cong
			// Cong dung: Doc file voi tham so la buffer, so ky tu cho phep va id cua file
			int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = kernel->machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
			int OldPos;
			int NewPos;
			char *buf;
			int tempt;
			// Thầy chỉ cho 20 file 
			if (id < 0 || id >= 20 )
			{
				printf("\nKhong the read vi id nam ngoai bang mo ta file.");
				kernel->machine->WriteRegister(2, -1); // -1: Loi
				IncreasePC();
				return;
				break;
			}
			// Kiem tra file co ton tai khong
			if (kernel->fileSystem->openfile[id] == NULL)
			{
				printf("\nKhong the read vi file nay khong ton tai.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				break;
			}
			// if (kernel->fileSystem->openfile[id]->type == 3) // Xet truong hop doc file stdout (type quy uoc la 3) thi tra ve -1
			// {
			// 	printf("\nKhong the read file stdout.");
			// 	kernel->machine->WriteRegister(2, -1);
			// 	IncreasePC();
			// 	return;
			// }
			OldPos = kernel->fileSystem->openfile[id]->GetCurrentPos(); // Sau khi ko có lỗi thì tiến hành kiểm tra vị trị hiện tại của file
			buf = User2System(virtAddr, charcount);                  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
			// Xet truong hop doc file stdin (type quy uoc la 2)
			//Còn tại sao phải đọc stdin như z thì tại vì stdin nó là một stream chứ ko phải là một file bình th
			if (id==0)
			{
				// Su dung ham Read cua lop SynchConsole de tra ve so byte thuc su doc duoc
				int size = 0;
				for (int i = 0; i < charcount; ++i)
				{
				size = size + 1;
				buf[i] = kernel->ReadChar(); //Lấy từng chữ 1
				//Quy uoc chuoi ket thuc la \n, Ko đọc hết file vì nó cũng như những cái read bình thường
				if (buf[i] == '\n')
				{
					buf[i + 1] = '\0';
					break;
				}
				}
				buf[size] = '\0';
				System2User(virtAddr, size, buf);        // Copy chuoi(buf) tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
				//Sau đó tham số buffer truyền vào sẽ mang chuỗi đó
				kernel->machine->WriteRegister(2, size); // Tra ve so byte thuc su doc duoc
				delete buf;
				IncreasePC();
				return;
				break;
			}
			//socket
			// Xet truong hop doc file binh thuong thi tra ve so byte thuc su
			if (kernel->fileSystem->openfile[id]->type == 0 || kernel->fileSystem->openfile[id]->type == 1) {
				if ((kernel->fileSystem->openfile[id]->Read(buf, charcount)) > 0) //Neu so bytes doc dc >0
					{
						// So byte thuc su = NewPos - OldPos
						NewPos = kernel->fileSystem->openfile[id]->GetCurrentPos();
						// Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
						System2User(virtAddr, NewPos - OldPos, buf); //bytes=newpos-oldpos
						kernel->machine->WriteRegister(2, NewPos - OldPos); //Viết bytes vào *giờ mới nhận ra viết có dấu đc:))
					}
					else
					{
						// Truong hop con lai la doc file co noi dung la NULL tra ve -2
						//printf("\nDoc file rong.");
						kernel->machine->WriteRegister(2, -1);
					}
			} else if (kernel->fileSystem->openfile[id]->type == 2 && kernel->fileSystem->openfile[id]->isConnect == true) { // socket
				tempt = kernel->fileSystem->openfile[id]->Read(buf, charcount);
				if (tempt >0 ) {
					System2User(virtAddr, tempt, buf); //bytes=newpos-oldpos
					kernel->machine->WriteRegister(2, tempt);
				} else {
					kernel->machine->WriteRegister(2, -1);
				}
			} else {
				kernel->machine->WriteRegister(2, -1);
			}
			delete buf;
			IncreasePC();
			return;
			
		}
		case SC_Write:
		{
			int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = kernel->machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
			int OldPos;
			int NewPos;
			int tempt;
			char *buf;
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id >= 20)
			{
				printf("\nKhong the write vi id nam ngoai bang mo ta file.");
				cout <<" id: " << id;
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (kernel->fileSystem->openfile[id] == NULL)
			{
				printf("\nKhong the write vi file nay khong ton tai.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
			if (id==0 || kernel->fileSystem->openfile[id]->type == 1)
			{
				printf("\nKhong the write file stdin hoac file only read.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			OldPos = kernel->fileSystem->openfile[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
			buf = User2System(virtAddr, charcount);                  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
			// Xet truong hop ghi file read & write (type quy uoc la 0) thi tra ve so byte thuc su

			if (id ==1) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
			{
				int lengthWrite = 0;
				//Truong hop dung thi vi tri chuoi bang 0
				while (buf[lengthWrite] != 0)
				{
				kernel->putChar(buf[lengthWrite]);
				lengthWrite++;
				//Truong hop chuoi vuot qua 255
				if (lengthWrite == 255) 
				{
					//XOA Buff
					delete[] buf;
					virtAddr = virtAddr + 255;
					//Cộng 255 vào vị trí hiện tại
					buf = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255
					lengthWrite = 0; //
				}
				}
				kernel->machine->WriteRegister(2, lengthWrite - 1); // Tra ve so byte thuc su write duoc
				delete buf;
				IncreasePC();
				return;
			}
			//docfile bth
			if (kernel->fileSystem->openfile[id]->type == 0 )
			{
				if ((kernel->fileSystem->openfile[id]->Write(buf, charcount)) > 0) //Nếu số bytes viết ra lớn hơn 0
				{
				// So byte thuc su = NewPos - OldPos
					NewPos = kernel->fileSystem->openfile[id]->GetCurrentPos();
					kernel->machine->WriteRegister(2, NewPos - OldPos); // Viết bytes vào thanh ghi 2

				} else {
					kernel->machine->WriteRegister(2, -1);
				}
			} else if (kernel->fileSystem->openfile[id]->type == 2 && kernel->fileSystem->openfile[id]->isConnect == true) {
				tempt = kernel->fileSystem->openfile[id]->Write(buf, charcount);
				if (tempt > 0) //Nếu số bytes viết ra lớn hơn 0
				{
				// So byte thuc su = NewPos - OldPos
					kernel->machine->WriteRegister(2, tempt); // Viết bytes vào thanh ghi 2
				} else {
					kernel->machine->WriteRegister(2, -1);
				}
			} else {
				kernel->machine->WriteRegister(2, -1);
			}
			delete buf;
			IncreasePC();
			return;
			
		}
		case SC_Seek:
		{
			//input: pos , id

			int pos=kernel->machine->ReadRegister(4);
			int id=kernel->machine->ReadRegister(5);
			if(id==0 || id==1 || id>=20 || kernel->fileSystem->openfile[id]==NULL ||  kernel->fileSystem->openfile[id]->type == 2)
			{
				cout<<"Unable to seek file"<<endl;
				kernel->machine->WriteRegister(2,-1);
				IncreasePC();
				return;
				break;
			}
			int len=kernel->fileSystem->openfile[id]->Length();
			if(pos>len)
			{
				cout<<"Invalid position"<<endl;
				kernel->machine->WriteRegister(2,-1);
				IncreasePC();
				return;
				break;
			}
			
			//pos=-1 move to the end of file
			if(pos==-1)
			{
				pos=len; //position=length of the file;
			}
			kernel->fileSystem->openfile[id]->Seek(pos);
			cout<<pos<<endl;
			cout<<"Success seek"<<endl;
			kernel->machine->WriteRegister(2,pos);
			IncreasePC();
			return;
			break;
		}
		case SC_Remove:
		{
			//Input: filename;
			int virtAddr=kernel->machine->ReadRegister(4);
			char *filename=User2System(virtAddr,MaxFileLength+1);
			if (strlen(filename) == 0)
			{
				printf("\n File name is not valid");
				DEBUG('a', "\n File name is not valid");
				kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
				delete filename;
				IncreasePC();
				return;
				break;
			}
			if (filename == NULL)  //Neu khong doc duoc
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
				delete filename;
				IncreasePC();
				return;
				break;
			}
			bool isOpen=false;
			for(int i=0; i<20; i++)
			{
				if(filename== kernel->fileSystem->filename[i])
				{
					isOpen=true;
					break;
				}
			}
			if(isOpen)
			{
				cout<<"File is opened, cannot remove"<<endl;
				kernel->machine->WriteRegister(2,-1);
				IncreasePC();
				return;
				delete [] filename;
				break;
			}
			if(kernel->fileSystem->Remove(filename))
			{
				cout<<"Remove success"<<endl;
				kernel->machine->WriteRegister(2,0);
				IncreasePC();
				return;
			}
			else 
			{
				cout<<"Remove unsuccessful"<<endl;
				kernel->machine->WriteRegister(2,-1);
				IncreasePC();
				return;
			}
			delete filename;
			break;

		}
		case SC_PrintString: {
		// input char[]
		// output None

			int virtAddr;
			char *buffer;
			virtAddr =kernel->machine->ReadRegister(4); // Lay dia chi cua tham so buffer
			buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho user space sang system space
			int length = 0;
			while (buffer[length] != 0)
				length++; // Dem do dai that su cua chuoi
				kernel->PrintBuffer(buffer,length);
			 // Goi ham Write cua SynchConsoleOutput de in
									// string ra man hinh
			delete buffer;
			IncreasePC();
			return;
			break;
		}
		case SC_ReadString: {
		
			// input char[] voi int length
			// output :None
			int virAddr; // khai bao dia chi nhan tu thanh ghi
			int length;
			int inputLength;
			char *strName;
			char c;

			virAddr = kernel->machine->ReadRegister(4); // lay dia chi tu thanh ghi (char buffer[] o user space)
			length = kernel->machine->ReadRegister(5); // lay dia chi tu thanh
			strName =new char[length]; // day se la bien buffer duoc tra ve cho nguoi dung
			inputLength = 0;
			while ((c = kernel->ReadChar()) != '\n') {
				strName[inputLength] = c;
				inputLength++;
			}
			strName[inputLength] = '\0';

			int numBytes=System2User(virAddr, inputLength, strName); // chuyen bo nho qua user
			if (inputLength > length) {
				printf("\nChuoi nhap co do dai qua lon so voi quy dinh mat roi\n");
				IncreasePC();
				return;
			}
			if (numBytes == 0) {
				printf("Chuoi rong\n");
			} else if (numBytes > MAX_LENGTH_STRING) {
				printf("\nChuoi qua lon so vs he thong");
				IncreasePC();
				return;
			}

			// clearInput();
			IncreasePC();
			return;
			break;
		}
		case SC_PrintInt:
		{
			int number;
			int temp;
			char *buffer;
			int countDigits, indexStart, MAX_SIZE_BUFFER;
			int i;
			countDigits = 0; // So digit cua number
			indexStart = 0;  // Vi tri bat dau in chu so

			MAX_SIZE_BUFFER =
				11; // 1: sign, 10: digits vi so nguyen kieu int toi da can 10 digits
			number = kernel->machine->ReadRegister(4);

			if (number == 0) {                       // truong hop de xu ly nhat
				kernel->putChar('0'); // In ra man hinh so 0
				IncreasePC();
				return;
			}

			/*Chuyen so thanh chuoi roi in ra man hinh*/
			if (number < 0) {

				number = -number; // Chuyen thanh so duong => easy
				indexStart = 1;
			}

			temp = number;   // bien tam cho number
			while (temp > 0) // dem so digits
			{
				countDigits++;
				temp /= 10;
			}

			// Tao buffer chuoi de in ra man hinh
			// int: [-2147483648, 2147483647]
			buffer = new char[MAX_SIZE_BUFFER + 1];
			for (i = countDigits - 1 + indexStart; i >= indexStart; i--) {
				buffer[i] = (char)((number % 10) + '0'); // Lay tung chu so vao buffer
				number /= 10;
			}
			if (indexStart == 1) { // Neu la so am
				buffer[0] = '-';
				buffer[countDigits + 1] = 0;
				for (i = 0; i <= countDigits; i++) {
				kernel->putChar(buffer[i]);
				}

				delete buffer;
			} else { // Neu la so duong
				buffer[countDigits] = 0;
				for (i = 0; i <= countDigits - 1; i++) {
				kernel->putChar(buffer[i]);
				}

				delete buffer;
			}
			IncreasePC();
			return;
			break;
		}
		case SC_Socket:
		{
				int tempt=kernel->fileSystem->isFull();
				if(tempt==-1)
				{
					printf("\n over 20 files/sockets are opened");
					kernel->machine->WriteRegister(2,-1);
					IncreasePC();
					return;
					break;
				}
				if((kernel->fileSystem->openfile[tempt]=kernel->fileSystem->socketTCP())!=NULL)
				{
					cout<<"Success open socket "<< endl;
					kernel->fileSystem->filename[tempt]= "socket";
					kernel->machine->WriteRegister(2,tempt); //index -1 vi after open thì nó đã cộng một vị trí index 
					// cout<<kernel->fileSystem->openfile[kernel->fileSystem->index-1]->type;
					
				}
				else
				{
					cout<<"Cannot open socket"<<endl;
					kernel->machine->WriteRegister(2,-1);
				}
				IncreasePC();
				return;
				break;
		}
		case SC_Connect:
		{ //INPUT: int socketid(index), char *ip, int port
			int socketid = kernel->machine->ReadRegister(4); // Lay socketid tu thanh ghi so 5
			int virtAddr = kernel->machine->ReadRegister(5);  // Lay dia chi cua tham so ip tu thanh ghi so 4
			int port = kernel->machine->ReadRegister(6);        // Lay port tu thanh ghi so 6
			char* ip;
			if (socketid < 2 || socketid >=20 )
			{
				printf("\nKhong the connect vi id nam ngoai bang mo ta file/socket.");
				kernel->machine->WriteRegister(2, -1); // -1: Loi
				IncreasePC();
				return;
				break;
			}
			if (kernel->fileSystem->openfile[socketid] == NULL)
			{
				printf("\nKhong the connect vi socket nay khong ton tai.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				break;
			}
			ip = User2System(virtAddr, MAX_LENGTH_IP); 
			if(kernel->fileSystem->openfile[socketid]->Connect(ip, port)<0){
				cout << ip;
				kernel->machine->WriteRegister(2, -1);
			} else {
				kernel->machine->WriteRegister(2, 0);
			}
			delete [] ip ;
			IncreasePC();
			return;
			break;
		}
		case SC_Send:
		{
			int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = kernel->machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
			int tempt;
			char *buf;
			if (id < 0 || id >= 20)
			{
				printf("\nKhong the send vi id nam ngoai bang mo ta file.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if (kernel->fileSystem->openfile[id] == NULL)
			{
				printf("\nKhong the send vi socket nay khong ton tai.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			buf = User2System(virtAddr, charcount);// Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
			if (kernel->fileSystem->openfile[id]->type == 2 && kernel->fileSystem->openfile[id]->isConnect == true) {
				tempt = kernel->fileSystem->openfile[id]->Write(buf, charcount);
				if (tempt > 0) //Nếu số bytes viết ra lớn hơn 0
				{
				// So byte thuc su = NewPos - OldPos
					kernel->machine->WriteRegister(2, tempt); // Viết bytes vào thanh ghi 2
				} else {
					kernel->machine->WriteRegister(2, -1);
				}
			} else {
				kernel->machine->WriteRegister(2, -1);
			}
			delete buf;
			IncreasePC();
			return;

		}
		case SC_Receive:
		{
			int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = kernel->machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
			int OldPos;
			int NewPos;
			char *buf;
			int tempt;
			// Thầy chỉ cho 20 file 
			if (id < 2 || id >= 20 )
			{
				printf("\nKhong the receive vi id nam ngoai bang mo ta socket.");
				kernel->machine->WriteRegister(2, -1); // -1: Loi
				IncreasePC();
				return;
				break;
			}
			// Kiem tra file co ton tai khong
			if (kernel->fileSystem->openfile[id] == NULL)
			{
				printf("\nKhong the receive vi socket nay khong ton tai.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				break;
			}
			buf = User2System(virtAddr, charcount); 
			if (kernel->fileSystem->openfile[id]->type == 2 && kernel->fileSystem->openfile[id]->isConnect == true) { // socket
				tempt = kernel->fileSystem->openfile[id]->Read(buf, charcount);
				if (tempt >0 ) {
					System2User(virtAddr, tempt, buf); //bytes=newpos-oldpos
					kernel->machine->WriteRegister(2, tempt);
				} else {
					kernel->machine->WriteRegister(2, -1);
				}
			} else {
				kernel->machine->WriteRegister(2, -1);
			}
			delete buf;
			IncreasePC();
			return;
		}
		case SC_Exec:
		{
			int virtAddr;
    		virtAddr = kernel->machine->ReadRegister(4);  // doc dia chi ten chuong trinh tu thanh ghi r4
    		char* name;
			name = User2System(virtAddr, MAX_LENGTH_PROCESS_NAME+1);
			if (name == NULL) {
				DEBUG(dbgSys, "\n Not enough memory in System");
				ASSERT(false);
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
    		}
			//kiểm tra có tồn tại file với name như trên không
			OpenFile* oFile = kernel->fileSystem->Open(name);
			if (oFile == NULL) {
				DEBUG(dbgSys, "\nExec:: Can't open this file.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			delete oFile;
			//thực thi việc chạy chương trình mới
			kernel->machine->WriteRegister(2, kernel->pTab->ExecUpdate(name));
			//delete []name;
			IncreasePC();
			return;
		}
		case SC_Join:
		{
			int id = kernel->machine->ReadRegister(4);
    		kernel->machine->WriteRegister(2, kernel->pTab->JoinUpdate(id));
			
			IncreasePC();
			return;		
		}
		case SC_Exit:
		{
			int exitcode = kernel->machine->ReadRegister(4);
			kernel->pTab->ExitUpdate(exitcode);
			kernel->machine->WriteRegister(2, 0);
			IncreasePC();
			return;
		}
		case SC_CreateSemaphore:
		{
			int virtAddr;
    		virtAddr = kernel->machine->ReadRegister(4);  // doc dia chi ten chuong trinh tu thanh ghi r4
    		char* name;
			name = User2System(virtAddr, MAX_LENGTH_PROCESS_NAME+1);
			if (name == NULL) {
				DEBUG(dbgSys, "\n Not enough memory in System");
				ASSERT(false);
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
    		}
			int semValue = kernel->machine->ReadRegister(5);

			kernel->machine->WriteRegister(2, kernel->semTab->Create(name,semValue));
			delete [] name;
			IncreasePC();
			return;
		}
		case SC_Signal:
		{
			int virtAddr;
    		virtAddr = kernel->machine->ReadRegister(4);  // doc dia chi ten chuong trinh tu thanh ghi r4
    		char* name;
			name = User2System(virtAddr, MAX_LENGTH_PROCESS_NAME+1);
			if (name == NULL) {
				DEBUG(dbgSys, "\n Not enough memory in System");
				ASSERT(false);
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
    		}
			kernel->machine->WriteRegister(2, kernel->semTab->Signal(name));
			delete []name;
			IncreasePC();
			return;
		}
		case SC_Wait:
		{
			int virtAddr;
    		virtAddr = kernel->machine->ReadRegister(4);  // doc dia chi ten chuong trinh tu thanh ghi r4
    		char* name;
			name = User2System(virtAddr, MAX_LENGTH_PROCESS_NAME+1);
			if (name == NULL) {
				DEBUG(dbgSys, "\n Not enough memory in System");
				ASSERT(false);
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
    		}
			kernel->machine->WriteRegister(2, kernel->semTab->Wait(name));
			delete []name;
			IncreasePC();
			return;
		}
		case SC_ExecV:
		{
			int argc = kernel->machine->ReadRegister(4);
            int addr = kernel->machine->ReadRegister(5);
			char **argv = new char*[argc + 1];
			//argv=User2System(argvAddr,MaxFileLength);
			for (int i = 0; i < argc; i++) {
				int argAddr;
				kernel->machine->ReadMem(addr + i * 4, 4, &argAddr);
				argv[i] = User2System(argAddr, MaxFileLength);
    		}
    		argv[argc] = NULL;
			
			kernel->machine->WriteRegister(2, kernel->pTab->ExecV(argc,argv));

			IncreasePC();
			return;
		}
    	default:
			cerr << "Unexpected system call " << type << "\n";
			ASSERTNOTREACHED();
			break;
      }
	  break;
	} 
    IncreasePC();
    // default:
    //   cerr << "Unexpected user mode exception" << (int)which << "\n";
    //   break;
	// }
    // ASSERTNOTREACHED();
	}
}
