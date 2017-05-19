// processmanager.h
//  The process manager is responsible for keeping track of the currently
//	running processes.
//

#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "bitmap.h"
#include "../threads/synch.h"
#include "pcb.h"

#define MAX_PROCESSES 8

class ProcessManager {

public:
    ProcessManager();
    ~ProcessManager();

    int allocPid();         			// Allocate a new PID
    void trackPcb(int pid, PCB *pcb);	// Allocate the accompany PCB to pid
    void freePid(int pid);  			// Free an allocated PID
    PCB* getPCB(int pid);
    int getNumPidAvail(){ return pids->NumClear();}

    void waitFor(int childPID);

    Lock *lock;

private:
    BitMap *pids;           			// Table to keep track of PIDs
    PCB **PCB_list;						// Table to keep track of PCBs
    Condition **exitSignals;            // Table to help broadcast exit signal
    Lock **exitLocks;                   // Accompanying Lock for CV
    int* wait_counts;                   // Keeping Track of how many processes waiting for exit code
};

#endif // PROCESSMANAGER_H