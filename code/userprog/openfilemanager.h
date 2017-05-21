		// openfilemanager.h
//

#ifndef OPENFILEMANAGER_H
#define OPENFILEMANAGER_H

#include "sysopenfile.h"
#include "synch.h"

#define OPEN_FILE_TABLE_SIZE 32
#define FAILED_TO_ADD -1
#define FAILED_TO_FIND -1

class OpenFileManager {

public:
    OpenFileManager();
    ~OpenFileManager();

    int addOpenFile(SysOpenFile* openFile);
    SysOpenFile *getOpenFile(int index);

    Lock *consoleWriteLock;
    
private:
    SysOpenFile* openFileTable[OPEN_FILE_TABLE_SIZE];
};

#endif // OPENFILEMANAGER_H