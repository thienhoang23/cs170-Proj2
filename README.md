# CS170 Project 2: Multiprogramming And File System

---

## TO DO LIST:
  * Part1: Process-oriented System Calls:
    * Fork: creates a new user-level process whose address space starts out as a copy of that of the caller.
    Used to execute function defined in the same file.
    * Yield(): relinquish CPU Time
    * Exit(Status): Terminate current process
    * **COMPLETED** ~~Exec(filename): Spawns a new user-level process to execute the file named.~~
    * Join(ID): waits and returns only after a process with the specified ID finishes and exit code collected.
    * **COMPLETED:** ~~Halt(): Turn off the machine~~
  * Part2: File-oriented System Calls:
    * Create()
    * Open()
    * Read()
    * Write():
      ** **COMPLETED:** ~~print directly to console~~
    * Close()

---

## FILES TO BE EDITED:
  * addrspace
  * exception
  * memorymanager
  * openfilemanager
  * pcb
  * processmanager
  * progtest
  * sysopenfile
  * useropenfile
  * P2_report
---

## ISSUE REMMAINNGS:
