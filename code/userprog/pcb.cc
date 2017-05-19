// pcb.cc
//

#include "pcb.h"

//----------------------------------------------------------------------
// PCB::PCB
//  Allocate a new process control block and initialize it with process
//  ID and a parent.
//----------------------------------------------------------------------

PCB::PCB(int pid, int parentPid)
{
    this->pid = pid;
    this->parentPid = parentPid;
    this->exitStatus = NOT_FINISHED;
}

//----------------------------------------------------------------------
// PCB::~PCB
//  Deallocate a process control block.
//----------------------------------------------------------------------

PCB::~PCB()
{
}

void PCB::setExitStatus(int status){ this-> exitStatus = status;}

int PCB::getExitStatus(){ return exitStatus;}