// openfilemanager.cc
//

#include "openfilemanager.h"

//----------------------------------------------------------------------
// OpenFileManager::OpenFileManager
//  Construct and initialize the open file manager.
//----------------------------------------------------------------------

OpenFileManager::OpenFileManager()
{
   consoleWriteLock = new Lock("consoleWriteLock");
}

//----------------------------------------------------------------------
// OpenFileManager::~OpenFileManager
//----------------------------------------------------------------------

OpenFileManager::~OpenFileManager()
{
    for(int i = 0; i < OPEN_FILE_TABLE_SIZE; i++){
		if (openFileTable[i] != NULL){
			delete openFileTable[i];
		}
	}
	delete[] openFileTable;
    delete consoleWriteLock;
}

//----------------------------------------------------------------------
// OpenFileManager::addOpenFile
//  Adds an on open file to the system file table.
//----------------------------------------------------------------------

int OpenFileManager::addOpenFile(SysOpenFile* openFile)
{
	for(int i = 0; i < OPEN_FILE_TABLE_SIZE; i++){
		if (openFileTable[i] == NULL){
			openFileTable[i] = openFile;
			return i;
		}
	}
	return FAILED_TO_ADD;
}

//----------------------------------------------------------------------
// OpenFileManager::getOpenFile
//  Retrieves the system file table entry from the file table.
//----------------------------------------------------------------------

SysOpenFile *OpenFileManager::getOpenFile(int index)
{
	if(openFileTable[index] == NULL)
		return FAILED_TO_FIND;
	return openFileTable[index];
}
