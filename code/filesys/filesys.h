// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#define MAX_FILES 100

#define _ConsoleInput 0
#define _ConsoleOutput 1

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {
  public:
  	//Level 2 pointer(OpenFile**) points to array of level 1 pointer(OpenFile*) 
	OpenFile** p;
	char fileNames[MAX_FILES][256];

    FileSystem() {
		p = new OpenFile*[MAX_FILES];
		
		int i = 0;
		while (i++ < MAX_FILES) {
			p[i] = NULL;
		}
		this->Create("stdin");
		this->Create("stdout");
		p[_ConsoleInput] = this->Open("stdin");
		p[_ConsoleOutput] = this->Open("stdout");
		strcpy(fileNames[_ConsoleInput], "stdin");
		strcpy(fileNames[_ConsoleOutput], "stdout");
	}

	~FileSystem()
	{
		for (int i = 0; i < MAX_FILES; i++)
			if (p[i])
			{
				delete p[i];
			}
		delete[]p;
	}

	int findFreeSlot()
	{
		for (int i = 2; i < MAX_FILES; i++)
			if (p[i] == NULL)
				return i;
		return -1;
	} 

	int Read(char* buffer, int size, int id){
		if(id < 0 || id > MAX_FILES) 
			return -1;
		if(p[id] == NULL ) 
			return -1;

		int resultSize = p[id]->Read(buffer, size);

		//we are reading less byte than the user ask us to read, so we need a different return value here.
		if(resultSize != size) 
			return -2;
		return 0;
	}

	int Seek(int seekPos, int id){
		if (id <= 1 || id > MAX_FILES)
			return -1;
		if (p[id] == NULL)
			return -1;
		if(seekPos < -1 || seekPos > p[id]->Length())
			return -1;

		//if seekPos == -1 means move to end of file
		if(seekPos == -1)
			seekPos = p[id]->Length();
		
		return p[id]->Seek(seekPos);
	}

	int Write(char* buffer, int size, int id){
		if(id < 0 || id > MAX_FILES) 
			return -1;
		if(p[id] == NULL ) 
			return -1;
		
		p[id]->Write(buffer, size);
		return 0;
	}


    bool Create(char *name) {
	int fileDescriptor = OpenForWrite(name);

	if (fileDescriptor == -1) return FALSE;
	Close(fileDescriptor); 
	return TRUE; 
	}

    OpenFile* Open(char *name) {
	  int fileDescriptor = OpenForReadWrite(name, FALSE);

	  if (fileDescriptor == -1) return NULL;
	  return new OpenFile(fileDescriptor);
    }
	 	
    bool Remove(char *name) { return Unlink(name) == 0; }
};

#else // FILESYS
class FileSystem {
  public:
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.

    bool Create(char *name, int initialSize);  	
					// Create a file (UNIX creat)

    OpenFile* Open(char *name); 	// Open a file (UNIX open)

    bool Remove(char *name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents

  private:
   OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
   OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
