#include <stdio.h>

#define M 3

int matrixOriginal[M][M] = {
    {1,2,3}, {4,5,6}, {7,8,9}
};
int matrixDes[M][M] = {0};

void produce ( int row, int column );

int main()

{

    int i , j;

    for (i=0;i<M;i++)

        for (j=0;j<M;j++)

              produce(i , j);

    // …;

}

void produce ( int row, int column )

{

    int i;

    for (i=0;i<M;i++)

        matrixDes[row][column] += matrixOriginal[row][i] * matrixOriginal[i][column];

}