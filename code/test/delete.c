#include"syscall.h"
int main(){
   PrintNum(Open("man.txt"));
   PrintNum(Remove("man.txt"));
   Halt();
}