// exception.cc
//  Entry point into the Nachos kernel from user programs.
//  There are two kinds of things that can cause control to
//  transfer back to here from user code:
//
//  syscall -- The user code explicitly requests to call a procedure
//  in the Nachos kernel.  Right now, the only function we support is
//  "Halt".
//
//  exceptions -- The user code does something that the CPU can't handle.
//  For instance, accessing memory that doesn't exist, arithmetic errors,
//  etc.
//
//  Interrupts (which can also cause control to transfer from user
//  code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include <stdio.h>
#include <string.h>
#include "copyright.h"
#include "syscall.h"
#include "system.h"
#include "machine.h"
#include "pcb.h"
#include "../threads/system.h"
#include "../machine/machine.h"
#include "addrspace.h"
#include "../filesys/openfile.h"

#define MAX_FILENAME 256

SpaceId doFork();
void ForkBridge(int newProcessPC);
void doExit();
int doExec();
void doWrite();
void doYield();
int doJoin();

//----------------------------------------------------------------------
// ExceptionHandler
//  Entry point into the Nachos kernel.  Called when a user program
//  is executing, and either does a syscall, or generates an addressing
//  or arithmetic exception.
//
//  For system calls, the following is the calling convention:
//
//  system call code -- r2
//    arg1 -- r4
//    arg2 -- r5
//    arg3 -- r6
//    arg4 -- r7
//
//  The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//  "which" is the kind of exception.  The list of possible exceptions
//  are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int result, type = machine->ReadRegister(2);

    if (which == SyscallException) {
        switch (type) {
            case SC_Halt:
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;
            case SC_Fork:
                DEBUG('a', "Fork() system call invoked.\n");
                result = doFork();
                machine->WriteRegister(2, result);
                break;
            case SC_Exit:
                DEBUG('a', "Exit() system call invoked.\n");
                doExit();
                break;
            case SC_Exec:
                DEBUG('a', "Exec() system call invoked.\n");
                result = doExec();
                machine->WriteRegister(2, result);
                break;
            case SC_Yield:
                DEBUG('a', "Yield() system call invoked.\n");
                doYield();
                break;
            case SC_Join:
                DEBUG('a', "Join() system call invoked.\n");
                result = doJoin();
                machine->WriteRegister(2, result);
                break;
            case SC_Write:
                DEBUG('a', "Write() system call invoked.\n");
                doWrite();
                break;
            default:
                printf("Unexpected system call %d. Halting.\n", type);
                interrupt->Halt();
        }
    } else {
        printf("Unexpected user mode exception %d. Halting.\n", which);
        interrupt->Halt();
    }

    // Increment program counter
    machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PrevPCReg) + 4);

}

//----------------------------------------------------------------------
// doExit
//----------------------------------------------------------------------

void doExit()
{
    //Get status code
    int status = machine->ReadRegister(4);
    //Set the exit status in the PCB of this process 
    int curPID = currentThread->space->getpid();
    PCB* curPCB = processManager -> getPCB(curPID);
    curPCB->setExitStatus(status);

    //Also let other processes know this process  exits.
    fprintf(stderr, "Process  %d is broadcasting exit\n", currentThread -> space -> getpid());    
    processManager -> broadcastExit(curPID);

    //Clean up the space of this process
    //fprintf(stderr, "Freeing addr space of  %d\n", currentThread -> space -> getpid());    
    delete currentThread->space;
    //fprintf(stderr, "Done Freeing");
    currentThread->space = NULL;
    
    //Terminate the current Nacho thread
    currentThread->Finish();
}

//----------------------------------------------------------------------
// execLauncher
//----------------------------------------------------------------------

void execLauncher(int unused)
{
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->Run();
}

//----------------------------------------------------------------------
// doExec
//----------------------------------------------------------------------

int doExec()
{
    int filenameVirtAddr = machine->ReadRegister(4);
    int filenamePhysAddr, bytesCopied = 0;
    char *filename = new char [MAX_FILENAME + 1], *kernelPtr = filename;
    char *userPtr, *endOfPage;

    PCB *childPcb;
    int parentPid, childPid;
    OpenFile *execFile;
    AddrSpace* childSpace;
    Thread* childThread;

    // First, we need to read the filename of the program to execute out of
    // user memory. This is complicated by the fact that the name might lie
    // across a page boundary.
    do {

        // Find this portion of filename in physical memory
        filenameVirtAddr += bytesCopied;
        filenamePhysAddr = currentThread->space->Translate(filenameVirtAddr);
        userPtr = machine->mainMemory + filenamePhysAddr;

        // Find the end of the page
        endOfPage = machine->mainMemory + (filenamePhysAddr / PageSize + 1)
                                           * PageSize;

        // Copy characters until the string or page ends
        while (*userPtr != '\0' && userPtr != endOfPage) {
            *kernelPtr++ = *userPtr++;
            ++bytesCopied;
        }

    } while (*userPtr != '\0' && bytesCopied != 0);

    *kernelPtr = '\0';

    // The new process needs a kernel thread by which we can manage its state
    childThread = new Thread("child of Exec()");

    // Finally it needs an address space. We will initialize the address
    // space by loading in the program found in the executable file that was
    // passed in as the first argument.
    execFile = fileSystem->Open(filename);
    if (execFile == NULL) {
        fprintf(stderr, "Unable to open file %s for execution. Terminating.\n", filename);
        return -1;
    }

    childSpace = new AddrSpace(execFile);

    // Initialize new PCB
    childPid = childSpace -> getpid();
    parentPid = currentThread -> space -> getpid();
    childPcb = new PCB(childPid, parentPid);

    processManager -> trackPCB(childPid, childPcb);

    // Link everything together
    childThread -> space = childSpace;
    childPcb -> thread = childThread;
    
    delete execFile;
    
    // We launch the process with the kernel threads Fork() function. Note
    // that this is different from our implementation of Fork()!
    childPcb->thread->Fork(execLauncher, 0);

    // Because we just overwrote the current thread's address space when we
    // called `new AddrSpace(execFile)` it can no longer be allowed to
    // execute so we call Finish(). You will have to fix this in your
    // implementation once you implement multiple address spaces.
    doYield();
    return childPid;
}

//----------------------------------------------------------------------
// doWrite
//----------------------------------------------------------------------

void doWrite()
{
    int userBufVirtAddr = machine->ReadRegister(4);
    int userBufSize = machine->ReadRegister(5);
    int dstFile = machine->ReadRegister(6);

    int i, userBufPhysAddr, bytesToEndOfPage, bytesToCopy, bytesCopied = 0;
    char *kernelBuf = new char[userBufSize + 1];

    if (dstFile == ConsoleOutput) {

        // Copy bytes from user memory into kernel memory
        while (bytesCopied < userBufSize) {

            // Perform virtual to physical address translation
            userBufPhysAddr = currentThread->space->Translate(userBufVirtAddr + bytesCopied);

            // Determine how many bytes we can read from this page
            bytesToEndOfPage = PageSize - userBufPhysAddr % PageSize;
            if (userBufSize < bytesToEndOfPage)
                bytesToCopy = userBufSize;
            else
                bytesToCopy = bytesToEndOfPage;

            // Copy bytes into kernel buffer
            memcpy(&kernelBuf[bytesCopied], &machine->mainMemory[userBufPhysAddr], bytesToCopy);
            bytesCopied += bytesToCopy;
        }

        // Write buffer to console (writes should be atomic)
        openFileManager->consoleWriteLock->Acquire();
        for (i = 0; i < userBufSize; ++i)
            UserConsolePutChar(kernelBuf[i]);
        openFileManager->consoleWriteLock->Release();
    }

    delete[] kernelBuf;
}

SpaceId doFork()
{
    // Sanity Check
    if (currentThread->space->getNumPages() > memoryManager->getNumFreeFrames() || 
        processManager->getNumPidAvail() <= 0)
        return -1;

    // Func1 address is in register 4
    int Func1Addr = machine->ReadRegister(4);

    // Create a new kernel thread for the child
    Thread* child_Thread = new Thread("child of Fork()");

    // Create a duplicate address space of current process
    AddrSpace *dupAddrSpace = new AddrSpace(currentThread->space);
    
    // Create Child PCB
    int parentPid, childPid;
    parentPid = currentThread->space->getpid();
    childPid = dupAddrSpace->getpid();

    //fprintf(stderr, "Process %d just forks process  %d\n", parentPid, childPid);

    PCB *childPcb = new PCB(parentPid, childPid);
    processManager-> trackPCB(childPid, childPcb);
    childPcb -> thread = child_Thread;
    childPcb -> thread -> space = dupAddrSpace;
    // New thread runs a dummy function creates a bridge for execution of the user function
    childPcb->thread->Fork(ForkBridge, Func1Addr);
    // Current thread Yield so new thread can run
    doYield();
    return childPid;
}

void ForkBridge(int newProcessPC)
{
    // Get fresh registers, but use copy of addr space
    currentThread->space->InitRegisters();
    // currentThread->RestoreUserState();
    currentThread->space->RestoreState();

    // Set the PC and run
    machine->WriteRegister(PCReg, newProcessPC);
    machine->WriteRegister(PrevPCReg, newProcessPC - 4);
    machine->WriteRegister(NextPCReg, newProcessPC + 4);

    machine->Run();
    ASSERT(FALSE); // should never reach here
}

void doYield()
{
  currentThread->SaveUserState();
  //This kernel thread yields
  currentThread->Yield();
  //Now this process is resumed for execution after yielding.
  //Restore the corresponding user process's states (both registers and page table)
  //Save the corresponding user process's register states.
  currentThread->RestoreUserState();
}


int doJoin()
{
    // Read the child Process Id
    int childPID = machine->ReadRegister(4);
    // Wait for child Process to Finish and get exitStatus
    int childExitStatus = processManager->waitFor(childPID);
    // Deallocate memory if currentThread is the parent Thread
    if( currentThread -> space -> getpid() == processManager -> getPCB(childPID) -> parentPid )
        processManager->freePid(childPID);
    // Return exit status
    return childExitStatus;
}
