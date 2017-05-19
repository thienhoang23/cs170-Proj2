// processmanager.cc
//

#include "processmanager.h"

//----------------------------------------------------------------------
// ProcessManager::ProcessManager
//  Create a new process manager to manager MAX_PROCESSES processes.
//----------------------------------------------------------------------

ProcessManager::ProcessManager()
{
    pids = new BitMap(MAX_PROCESSES);
    lock = new Lock("ProcessManagerLock");
    PCB_list = new PCB*[MAX_PROCESSES];
    for(int i = 0; i < MAX_PROCESSES; i++){
    	PCB_list[i] = NULL;
    }
}

//----------------------------------------------------------------------
// ProcessManager::~ProcessManager
//  Deallocate a process manager.
//----------------------------------------------------------------------

ProcessManager::~ProcessManager()
{
    delete pids;
    delete [] PCB_list;
    delete lock;
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