// pcb.h
//  The process control block is a data structure used to keep track of a
//	single user process. For now, each process has an ID, a parent and a
//	pointer to its kernel thread.

#ifndef PCB_H
#define PCB_H

class Thread;

#define NOT_FINISHED -1

class PCB {

public:
    PCB(int pid, int parentPid);
    ~PCB();

    void setExitStatus(int status);

    int getExitStatus();

    int exitStatus;
    int pid;            	// Process ID
    int parentPid;      	// Parent's Process ID
    Thread *thread;     	// Kernel thread that controls this process has the registers, 
    						// PC, and address_space
};

#endif // PCB_H
