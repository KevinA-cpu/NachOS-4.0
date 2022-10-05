#include "syscall.h"

int main(){
    PrintString("Team members in this NACHOS project include: \n");
    PrintString("20120265 - Bui Tien Dat.\n");
    PrintString(" - Nguyen Hoang Cao Son.\n");
    PrintString("20120253 - Ly Bang.\n");
    PrintString("Subject: Operating System.\n");
    PrintString("Class: 20_22.\n");
    PrintString("The team is full and hopefully remain the same until the end of the subject.\n");
    PrintChar('\n');
    PrintString("The following is a summary of c programs ascii.c and bbsort.c.\n");
    PrintString("From the code/test folder, run the following command in the terminal: \n");
    PrintString("    - '../build.linux/nachos -x ascii' to print the ASCII table code, not including the extended version.\n");
    PrintString("    - '../build.linux/nachos -x bbsort' to sort an array with n element using bubble sort.\n");
    PrintString("       The user will be required to enter the following when running bbsort.c: \n");
    PrintString("           + n: the number of elements in the array (0 <= n <= 100).\n");
    PrintString("           + a[i]: enter a number in the i position of the array.\n");
    PrintString("           + choosing a sort order (1: increasing or 2: decreasing).\n");
    Halt();
}