#include"syscall.h"
#define MAX_INPUT 256

int main(){
   char filename[MAX_INPUT];
   PrintString("Input filename: ");
   ReadString(filename, MAX_INPUT);
   PrintNum(Remove(filename));
   Halt();
}