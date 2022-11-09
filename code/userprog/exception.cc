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
void movePC(){
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
}

char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;

	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;

	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;

		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int addr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return 0;
	int i = 0, ch = 0;
	do
	{
		ch = (int)buffer[i];
		kernel->machine->WriteMem(addr + i, 1, ch);
		++i;
	} while (i < len && ch != 0);
	return i;
}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
	
    switch (which) {
	case NoException:	// return control to kernel
		kernel->interrupt->setStatus(SystemMode);
		DEBUG(dbgSys, "Switch to kernel mode\n");
        break;
	case PageFaultException:    // No valid translation found
		cerr << "No valid translation found\n";
        SysHalt();
        ASSERTNOTREACHED();
	case ReadOnlyException:     // Write attempted to page marked "read-only"
		cerr << "Write attempted to page marked 'read-only'\n";
        SysHalt();
        ASSERTNOTREACHED();
	case BusErrorException:     // Translation resulted in an invalid physical address
		cerr << "Translation resulted in an invalid physical address\n";
        SysHalt();
        ASSERTNOTREACHED();
	case AddressErrorException: // Unaligned reference or one that was beyond the end of the address space
		cerr << "Unaligned reference or one that was beyond the end of the address space\n";
        SysHalt();
        ASSERTNOTREACHED();
	case OverflowException:     // Integer overflow in add or sub.
		cerr << "Integer overflow in add or sub.\n";
        SysHalt();
        ASSERTNOTREACHED();
	case IllegalInstrException: // Unimplemented or reserved instr.
		cerr << "Unimplemented or reserved instr.\n";
        SysHalt();
        ASSERTNOTREACHED();
	case NumExceptionTypes:
		cerr << "Number Exception Types\n";
        SysHalt();
        ASSERTNOTREACHED();
    case SyscallException:
      switch(type) {
		case SC_ReadNum:
		{
			int num = SysReadNum();
			kernel->machine->WriteRegister(2, num);
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintNum:
		{
			int print = kernel->machine->ReadRegister(4);
			SysPrintNum(print);
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
		case SC_ReadChar:
		{
			char rs;
			rs = SysReadChar();
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)rs);
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintChar:
		{
			SysPrintChar((char)kernel->machine->ReadRegister(4));
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
		case SC_RandomNum:
		{
			DEBUG(dbgSys, "Random num \n");
			int result = SysRandomNum();
			kernel->machine->WriteRegister(2, (int)result);
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
		case SC_ReadString:
		{
			int addr;
			char *buffer;
			int length;

			addr = kernel->machine->ReadRegister(4);   // get buffer' address
			length = kernel->machine->ReadRegister(5); // maximum length of input string
			buffer = User2System(addr, length);		   // copy string from User space to Kernel space
			SysReadString(buffer, length);		   // system read string
			System2User(addr, length, buffer);		   // return string to User space
			delete[] buffer;
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintString:
		{
			int addr = kernel->machine->ReadRegister(4);
			char *buffer = User2System(addr, MAX_STRING);
			SysPrintString(buffer);
			delete[] buffer;
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
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
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
		case SC_Remove:
		{
			int addr= kernel->machine->ReadRegister(4);
			char* buffer = User2System(addr, MAX_STRING);
			int result = SysRemove(buffer);
			kernel->machine->WriteRegister(2, result);
			return movePC();

			ASSERTNOTREACHED();
			break;
		}
		case SC_Create:
		{
			DEBUG(dbgSys, "Creating file " << (char *)kernel->machine->ReadRegister(4));
			int addr = kernel->machine->ReadRegister(4);  // Get the address of the buffer of user space
			char *sysBuff = User2System(addr, MAX_STRING); // copy buffer from user space to kernel space
			int result = SysCreateFile(sysBuff);
			kernel->machine->WriteRegister(2, result);
			if (result == 0)
				printf("Creation successful\n");
			else
				printf("Creation failed\n");
			DEBUG(dbgSys, "Creation completed");

			return movePC();

			ASSERTNOTREACHED();

			break;
		}
		case SC_Open:
		{
			int addr = kernel->machine->ReadRegister(4);
			char *name = User2System(addr, 256);

			int result = SysOpenFile(name);
			DEBUG(dbgSys, "Passed SysOpen()\n");
			DEBUG(dbgSys, "result:");
			DEBUG(dbgSys, result);
			if (result > 0)
			{
				DEBUG(dbgSys, "Creation completed. ID = " << result);
			}
			else
			{
				DEBUG(dbgSys, "Creation failed");
			}
			kernel->machine->WriteRegister(2, result);

			return movePC();

			ASSERTNOTREACHED();

			break;
		}
		case SC_Close:
		{
			//
			DEBUG(dbgSys, "Closing file " << (char *)kernel->machine->ReadRegister(4));

			int fileid = kernel->machine->ReadRegister(4);

			int result = SysCloseFile(fileid);

			if (result == 0)
			{
				DEBUG(dbgSys, "Close completed.");
			}
			else
			{
				DEBUG(dbgSys, "Close failed");
			};
			kernel->machine->WriteRegister(2, result);

			return movePC();

			ASSERTNOTREACHED();

			break;
		}
		case SC_Read:
		{
			int virtAddr = machine->ReadRegister(4);
			int size = machine->ReadRegister(5);
			int id = machine->ReadRegister(6);
			int OldPos;
			int NewPos;
			char *buf;
			if (id < 0 || id > 14)
			{
				printf("\nerro open.");
				machine->WriteRegister(2, -1);
				return movePC();
			}
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nnot found.");
				machine->WriteRegister(2, -1);
				return movePC();
			}
			if (fileSystem->openf[id]->type == 3)
			{
				printf("\ncan't read file stdout.");
				machine->WriteRegister(2, -1);
				return movePC();
			}
			OldPos = fileSystem->openf[id]->GetCurrentPos();
			buf = User2System(virtAddr, size);
			if (fileSystem->openf[id]->type == 2)
			{
				int size = gSynchConsole->Read(buf, size); 
				System2User(virtAddr, size, buf);
				machine->WriteRegister(2, size);
				delete buf;
				return movePC();
			}
			if ((fileSystem->openf[id]->Read(buf, size)) > 0)
			{
				NewPos = fileSystem->openf[id]->GetCurrentPos();
				System2User(virtAddr, NewPos - OldPos, buf); 
				machine->WriteRegister(2, NewPos - OldPos);
			}
			else
			{
				machine->WriteRegister(2, -2);
			}
			delete buf;
			return movePC();
		}
			ASSERTNOTREACHED();
			break;
		
		case SC_Write:
		{
			int virtAddr = machine->ReadRegister(4);
			int size = machine->ReadRegister(5);
			int id = machine->ReadRegister(6);
			int OldPos;
			int NewPos;
			char *buf;            
			if (id < 0 || id > 14)
			{
				printf("\nerro.");
				machine->WriteRegister(2, -1);
				return movePC();
			}
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nnot found.");
				machine->WriteRegister(2, -1);
				return movePC();
			}
			if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
			{
				printf("\nfile only read.");
				machine->WriteRegister(2, -1);
				return movePC();
			}
			OldPos = fileSystem->openf[id]->GetCurrentPos();
			buf = User2System(virtAddr, size); 
			if (fileSystem->openf[id]->type == 0)
			{
				if ((fileSystem->openf[id]->Write(buf, size)) > 0)
				{
					NewPos = fileSystem->openf[id]->GetCurrentPos();
					machine->WriteRegister(2, NewPos - OldPos);
					delete buf;
					return movePC();
				}
			}
			if (fileSystem->openf[id]->type == 3)
			{
				int i = 0;
				while (buf[i] != 0 && buf[i] != '\n')
				{
					gSynchConsole->Write(buf + i, 1);
					i++;
				}
				buf[i] = '\n';
				gSynchConsole->Write(buf + i, 1);
				machine->WriteRegister(2, i - 1);
				delete buf;
				return movePC();
			}
			ASSERTNOTREACHED();
			break;
		}
      	default:
			cerr << "Unexpected system call " << type << "\n";
			break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
