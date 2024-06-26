/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"
#include <limits.h>
#include <stdio.h>

#define MAX_STRING 512

/*
LF - Line Feed is a character indicating moving down one line
CR - Carriage Return is a character reseting to cursor to the begining of a line of text
HT - Horizontal Tab is a character creating horizontal white space pressing the Tab button
SP - Space is the white space character
All of this information and the defines below are all taken from ASCII table code
*/
#define LF (char)10
#define CR (char)13
#define HT (char)9
#define SP (char)32

//Max length of a minus integer, deprive from -2147483648
#define MAX_INTMINUS_LENGTH 11
//Max length of a plus integer, deprive from 2147483648
#define MAX_INTPLUS_LENGTH 10

//Buffer to store number
//The +1 is for the extra LF/CR/HT/SP character
char numBuffer[MAX_INTMINUS_LENGTH + 1];

//Check for the end of a number, usually a white space or EOF 
bool isEnd(char c) { 
  if(c == LF || c == CR || c == HT || c == SP)
    return true;
  return false;
}

//Check for integer Overflow/Underflow
bool isOvUn(int* result, int a, int b){
  *result = a + b;
  if(a > 0 && b > 0 && *result < 0)
      return true;
  if(a < 0 && b < 0 && *result > 0)
      return true;
  return false;
}

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysReadNum()
{
  //Set all character to 0
  memset(numBuffer, 0, sizeof(numBuffer));
  char c = kernel->synchConsoleIn->GetChar();
  /*
    Because we will need to read the number from the console until
    encounter EOF/LF/CR/HT/SP character, we must check for these chars
    at the beginning to avoid conflicting with the rest of the code below.
  */

  if (c == EOF) {
    DEBUG(dbgSys, "EOF at start, no number will be read.");
    return 0;
  }

  if (isEnd(c)) {
    DEBUG(dbgSys, "White space at start, no number will be read");
    return 0;
  }

  int i = 0;
  int minusCount = 0;
  while (!(isEnd(c) || c == EOF)) {
      //Check if the length of the number exceed integer type
      if (i >= MAX_INTMINUS_LENGTH && numBuffer[0] == '-') {
        DEBUG(dbgSys, "Number length exceed integer type.");
        return 0;
      }
      else if(i >= MAX_INTPLUS_LENGTH && numBuffer[0] != '-'){
        DEBUG(dbgSys, "Number length exceed integer type.");
        return 0;
      }
        
      //Check if the char is a valid digit (except for '-')
      if (!isdigit(c) && c != '-'){
        DEBUG(dbgSys, "Invalid number.");
        return 0;
      }

      if (c == '-')
        minusCount++;
      
      //Catch the --num invalid format
      if(minusCount > 1){
        DEBUG(dbgSys, "Invalid number format.");
        return 0;
      }

      numBuffer[i] = c;
      c = kernel->synchConsoleIn->GetChar();
      i++;
  }

  int length = strlen(numBuffer);

  //Check if the user even enter anything
  if (length == 0) 
    return 0;

  /* 
  The number -2147483648 and 2147483648 are too finicky to deal with
  so we have decide to handle them separately.
  */
  if (strcmp(numBuffer, "-2147483648") == 0) 
    return INT_MIN;
  else if (strcmp(numBuffer, "2147483648") == 0)
    return INT_MAX;

  int pos = 0;
  int number = 0;

  //Change starting position depends on whether the number is negative or positive
  if(numBuffer[0] == '-')
    pos = 1;
  else
    pos = 0;

  for (int i = pos; i < length; i++) {
    char temp = numBuffer[i];

  //Catch the 00/-0 which are not acceptable format, careful not to catch the number 0 alone
  if((temp == '0' && ((i == 0 && length != 1)|| (i == 1 && numBuffer[0] == '-')))){
    DEBUG(dbgSys, "Invalid number format.");
    return 0;
  }

  //Check if the number is overflowing/underflowing
  if(isOvUn(new int[sizeof(int)], number * 10, temp - '0') == true){
    DEBUG(dbgSys, "Integer value exceed the int type.");
    return 0;
  }
  //Overflow/Underflow can also happen during the process of converting char array to int
  else if((number > 0 && (number * 10 < 0)) || (number < 0 && (number * 10 > 0))){
    DEBUG(dbgSys, "Integer value exceed the int type.");
    return 0;
  }
  number = (number * 10) + (temp - '0');
}

  if (numBuffer[0] == '-')
    number = -number;

  return number;
}

void SysPrintNum(int num) 
{
  if (num == 0){
    kernel->synchConsoleOut->PutChar('0');
    return;
  }
  else if (num == INT_MIN) {
    kernel->synchConsoleOut->PutChar('-');
    for (int i = 0; i < 10; i++)
        kernel->synchConsoleOut->PutChar("2147483648"[i]);
    return;
  }
  else if(num == INT_MAX){
    for (int i = 0; i < 10; i++)
        kernel->synchConsoleOut->PutChar("2147483648"[i]);
    return;
  }
  else if (num < 0) {
    kernel->synchConsoleOut->PutChar('-');
    num = -num;
  }
    int n = 0;
    while (num) {
      numBuffer[n] = num % 10;
      num /= 10;
      n++;
    }
    for (int i = n - 1; i >= 0; i--)
      kernel->synchConsoleOut->PutChar(numBuffer[i] + '0');
    return;
}

char SysReadChar()
{
    return kernel->synchConsoleIn->GetChar();
}

void SysPrintChar(char ch)
{
  kernel->synchConsoleOut->PutChar(ch);
}

int SysRandomNum()
{
  RandomInit((unsigned)time(NULL));
  return RandomNumber();
}

void SysReadString(char *buffer, int length)
{
  int i;
  char c;

  for (i = 0; i < length; ++i)
      buffer[i] = 0;

  for (i = 0; i < length;)
  {
      do
      { 
          c = SysReadChar();
      } while (c == EOF);
      if (c == '\001' || c == '\n') // finish input
          break;
      buffer[i++] = c;
  }
}

void SysPrintString(char *buffer)
{
  for (int i = 0; buffer[i] != '\0'; i++)
  {
      kernel->synchConsoleOut->PutChar(buffer[i]);
  }
}

int SysCreateFile(char *name)
{
    if (strlen(name) == 0)
        return -1;
    if (!name)
        return -1;
    #ifdef FILESYS_STUB 
    bool code = kernel->fileSystem->Create(name); // Create file
    #else
    bool code = kernel->fileSystem->Create(name,0); // Also create file but keep the compiler happy
    #endif
    return code ? 0 : -1;                            // Return status code
}

OpenFileId SysOpenFile(char *name)
{
    if (strcmp(name, "stdin") == 0)
    {
        return _ConsoleInput;
    }
    if (strcmp(name, "stdout") == 0)
        return _ConsoleOutput;

    int freeSlot = kernel->fileSystem->findFreeSlot();
    if (freeSlot != -1) // Chi xu li khi con slot trong
    {
            kernel->fileSystem->p[freeSlot] = kernel->fileSystem->Open(name);
            if (kernel->fileSystem->p[freeSlot] != NULL) // Mo file thanh cong
            {   
                strcpy(kernel->fileSystem->fileNames[freeSlot], name);
                delete[] name;
                return freeSlot;
            }
    }
    delete[] name;
    return -1;
}

int SysCloseFile(OpenFileId id)
{
    if (id >= 0 && id <= MAX_FILES)
    {
        if (kernel->fileSystem->p[id])
        {
            delete kernel->fileSystem->p[id];
            kernel->fileSystem->p[id] = NULL;
            strcpy(kernel->fileSystem->fileNames[id], "");
            return 0;
        }
    }
    return -1;
}

int SysRead(char* buffer, int size, OpenFileId id){
  //id == 0 means file name is "stdin" which means reading from console.
  if(id == 0){
    SysReadString(buffer, size);
    return 0;
  }
  
  return kernel->fileSystem->Read(buffer, size, id);
}

int SysWrite(char* buffer, int size, OpenFileId id){
  //id == 1 means file name is "stdout" which means writting from console.
  if(id == 1){
    SysPrintString(buffer);
    return 0;
  }

  return kernel->fileSystem->Write(buffer, size, id);
}

int SysSeek(int seekPos, int id){
  if(id <= 1){
      DEBUG(dbgSys, "seek position in console is impossible");
      return -1;
  }
  return kernel->fileSystem->Seek(seekPos, id);
}

int SysRemove(char *name)
{
  //If somehow the char arry can't be initialize
  if(name == NULL){
    DEBUG(dbgSys, "Not enough memory for file name.\n");
		return -1;
  }

  //Check if the file is opened or not
  for(int i = 2; i < MAX_FILES;i++){
    if(strcmp(name, kernel->fileSystem->fileNames[i]) == 0){
      DEBUG(dbgSys, "Can't remove a file that is being opened.\n");
			return -1;
    } 
  }

  //Unexpected error like the file don't exists
	if(kernel->fileSystem->Remove(name) == false){
			DEBUG(dbgSys, "Can't remove the file, something is wrong.\n");
			return -1;
	}

	DEBUG(dbgSys, "Remove file successfully.\n");
	return 0;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
