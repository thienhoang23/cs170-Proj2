// processmanager.cc
//

#include "processmanager.h"
#include <string>
//----------------------------------------------------------------------
// ProcessManager::ProcessManager
//  Create a new process manager to manager MAX_PROCESSES processes.
//----------------------------------------------------------------------

ProcessManager::ProcessManager()
{
    pids = new BitMap(MAX_PROCESSES);
    lock = new Lock("ProcessManagerLock");
    PCB_list = new PCB*[MAX_PROCESSES];
    exitLocks = new Lock*[MAX_PROCESSES];
    exitSignals new Condition*[MAX_PROCESSES];
    wait_counts = new int[MAX_PROCESSES];

    for(int i = 0; i < MAX_PROCESSES; i++){
    	PCB_list[i] = NULL;
        exitLocks[i] = new Lock(std::to_string(i));
        exitSignals[i] = new Condition(std::to_string(i));
        wait_counts[i] = 0;
    }
}

//----------------------------------------------------------------------
// ProcessManager::~ProcessManager
//  Deallocate a process manager.
//----------------------------------------------------------------------

ProcessManager::~ProcessManager()
{
    delete pids;
    for(int i = 0; i < MAX_PROCESSES; i++){
        if PCB_list[i] != NULL
            delete PCB_list[i];
        delete exitLocks[i];
        delete exitSignals[i];
    }
    delete lock;
    delete[] wait_counts;
}

//----------------------------------------------------------------------
// ProcessManager::allocPid
//  Allocate an unused PID to be used by a process.
//
//  For now do nothing.
//----------------------------------------------------------------------

int ProcessManager::allocPid()
{
	int pid = pids -> Find();
	return pid;
}

//----------------------------------------------------------------------
// ProcessManager::trackPcb
//  Kepp track of pcb
//
//----------------------------------------------------------------------

void ProcessManager::trackPcb(int pid, PCB *pcb)
{
	this->PCB_list[pid] = pcb;
}

//----------------------------------------------------------------------
// ProcessManager::freePid
//  Deallocate a PID that is in use so that it can be allocated again by
//  another process.
// 	Deallocate PCB spot
//----------------------------------------------------------------------

void ProcessManager::freePid(int pid)
{
	pids -> Clear(pid);
	delete PCB_list[pid];
	PCB_list[pid] = NULL; 
}

//----------------------------------------------------------------------
// ProcessManager::getPCB
//  Do what its name suggests it do. Give back pointer to PCB
//
//----------------------------------------------------------------------

PCB* ProcessManager::getPCB(int pid)
{
	return PCB_list[pid]; 
}

//----------------------------------------------------------------------
// ProcessManager::waitFor
//  Wait on Condition Variable
//
//----------------------------------------------------------------------

void ProcessManager::waitFor(int childPID)
{
    exitLocks[childPID] -> Acquire();
    while(PCB_list[childPID]->getExitStatus() == NOT_FINISHED){
        wait_counts[childPID]++;
        Condition[childPID] -> Wait(exitLocks[childPID]);
        wait_counts[childPID]--;
    }
     exitLocks[childPID] -> Release();
}