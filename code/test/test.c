#include"syscall.h"
#define MAX_SIZE 101
int main(){
    char s[MAX_SIZE];
    ReadString(s, MAX_SIZE);
    PrintString(s);
    Halt();
}