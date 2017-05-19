// pcb.h
//  The process control block is a data structure used to keep track of a
//	single user process. For now, each process has an ID, a parent and a
//	pointer to its kernel thread.

#ifndef PCB_H
#define PCB_H

class Thread;

#define P_GOOD    0;
#define P_BAD     1;
#define P_RUNNING 2;
#define P_BLOCKED 3;

class PCB {

public:
    PCB(int pid, int parentPid);
    ~PCB();

    int pid;            	// Process ID
    int parentPid;      	// Parent's Process ID
    Thread *thread;     	// Kernel thread that controls this process has the registers, 
    						// PC, and address_space
};

#endif // PCB_H
