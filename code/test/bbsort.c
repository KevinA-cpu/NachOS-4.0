#include "syscall.h"
#define MAX_SIZE 100

int increasing(int left, int right)
{
    return left > right;
}

int decreasing(int left, int right)
{
    return left < right;
}

void swap(int *a, int *b)
{
    int c = *a;
    *a = *b;
    *b = c;
}

void bubbleSort(int *arr, int n, int (*comparisionFunc)(int, int))
{
    int i, j;
    for (i = 0; i < n - 1; i++)
    {
        // Last i elements are already in place
        for (j = 0; j < n - i - 1; j++)
        {
            if (comparisionFunc(arr[j], arr[j+1]))
            {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

int main() {
    int n, a[MAX_SIZE], order, temp, i, j;
    do {
        PrintString("Enter n (0 <= n <= 100): ");
        n = ReadNum();
        if (n < 0 || n > 100)
            PrintString("Wrong input of n, please try again\n");
    } while (n < 0 || n > 100);

    for (i = 0; i < n; i++) {
        PrintString("Enter a[");
        PrintNum(i);
        PrintString("]: ");
        a[i] = ReadNum();
    }

    do {
        PrintString("Enter sort order (1: increasing, 2: decreasing): ");
        order = ReadNum();
        if (order != 1 && order != 2)
            PrintString("Wrong input for sort order, please try again\n");
    } while (order != 1 && order != 2);

    if (order == 1)
    {
        bubbleSort(a, n, increasing);
    }
    else if (order == 2)
    {
        bubbleSort(a, n, decreasing);
    }

    PrintString("Sorted array: ");
    for (i = 0; i < n; i++) {
        PrintNum(a[i]);
        PrintChar(' ');
    }

    Halt();
}