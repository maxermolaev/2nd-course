#include <stdio.h>

#define SIZE 199999

int main() {
    int arr1[SIZE], arr2[SIZE], result[SIZE];

    for (int i = 0; i < SIZE; i++) {
        arr1[i] = i;
        arr2[i] = SIZE - i;
    }
    //без раскрутки
    //for (int i = 0; i < SIZE; i++) {
      //result[i] = arr1[i] + arr2[i];
    int i;
    //раскрутка
    for (i = 0; i < SIZE - 4; i += 4) {
        result[i] = arr1[i] + arr2[i];
        result[i + 1] = arr1[i + 1] + arr2[i + 1];
        result[i + 2] = arr1[i + 2] + arr2[i + 2];
        result[i + 3] = arr1[i + 3] + arr2[i + 3];   
    }
    for(i;i<SIZE;i++){
      result[i]=arr1[i]+arr2[i];
    }

    printf("%d\n", result[99999]);

    return 0;
}
