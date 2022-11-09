#include"syscall.h"
#define MAX_INPUT 256

int main(){
   char fileName[MAX_INPUT];
   PrintString("Input filename: ");
   ReadString(fileName, MAX_INPUT);
   PrintNum(Remove(fileName));
   Halt();
}