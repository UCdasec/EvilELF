#include<stdio.h>

int h;

void addOne(int* x){
    *x = *x + 1;
}

int main(int argc, char** argv){
    int x = 2;

    addOne(&x);
    printf("x = %d\n",x);
    return 0;
}

