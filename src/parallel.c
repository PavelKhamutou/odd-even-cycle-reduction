#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
int SIZE;

void print_matrix(double m[5][SIZE]) { 
    int i, j = 0;
    for (i = 0; i < 5; i++) {
        printf("\n\t| "); 
            for (j = 0; j < SIZE; j++) {
                printf("%2e ", m[i][j]);
            }
        printf("|");
    }
    printf("\n");
}

int check_size(const int size) {
    if (size % 2 == 0) {
        return 1;
    } else if (size == 3) {
        return 0;
    } else {
        int i;
        int new_size = 0;
        for (i = 1; i < size; i++) {
            if (i % 2 == 0) {
                new_size++;
            }
        }
        check_size(new_size);
    }
}

int main(int argc, char *argv[]) {
    int i, j, k, size, index;
    int index1, index2;
    int mynode, totalnodes;
    double alpha, gamma;
    const int numrows = 5;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &totalnodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
    /*
        size is equal to 2^(log2(totalnodes + 1) + 1) - 1
        for example for totalnodes equal to 3 we have 2^(log2(3 + 1) + 1) - 1 = 2^3 - 1 = 8 - 1 = 7
        not all values are allowed. matrix must have size equal to odd integer (1, 3, 5, 7)
     */
    size = 5; //(int) round(pow(2, log2(totalnodes + 1) + 1) - 1);
    SIZE = size + 1;
    /*if (mynode == 0) {
        if (check_size(size) == 1) {
            printf("Incorrect size: %d\n", size);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        printf("A size = %d\n", size);
        printf("totalnodes = %d\n", totalnodes);
    }*/
    double A[numrows][size + 1];
    for (i = 0; i < numrows; i++) {
        for (j = 0; j < size + 1; j++) {
            A[i][j] = 0.0;
        }
    }
    // filling A matrix
    if (mynode == 0) {
        A[0][0] = 2.0; A[0][1] = 1.0;
        A[1][0] = 1.0; A[1][1] = 2.0; A[1][2] = 1.0;
        A[2][1] = 1.0; A[2][2] = 2.0; A[2][3] = 1.0;
    } else if (mynode == (totalnodes - 1)) {
        index = 2 * mynode;
        A[0][index - 1] = 1.0; A[0][index] = 2.0; A[0][index + 1] = 1.0;
        index = 2 * mynode + 1;
        A[1][index - 1] = 1.0; A[1][index] = 2.0; A[0][index + 1] = 1.0;
        A[2][size - 2] = 1.0; A[2][size - 1] = 1.0;
    } else {
        for (i = 0; i < 3; i++) {
            index = i + 2 * mynode;
            A[i][index - 1] = 1.0; A[i][index] = 2.0; A[i][index + 1] = 1.0;
        }
    }
    // filling F vector 
    for (i = 0; i < 3; i++) {
        if (mynode == (totalnodes - 1) && i == 2 && size % 2 == 0) {
            A[i][size] = 0.0;
        } else {
            A[i][size] = 2 * mynode + i + 1;
        }
    }

    int numactivep = totalnodes;
    int activep[totalnodes];
    int lastProcess = totalnodes - 1;
    for (j = 0; j < numactivep; j++) {
        activep[j] = j;
    }

    for (j = 0; j < size + 1; j++) {
        A[3][j] = A[0][j];
        A[4][j] = A[2][j];
    }

    if (mynode == 0 || mynode == 1) {
        print_matrix(A);
    }

    for (i = 0; i < log2(size + 1) - 1; i++) {
        for (j = 0; j < numactivep; j++) {
            if (mynode == activep[j]) {
                index1 = 2 * mynode + 1 - pow(2, i); //2 + 1 - 2 = 1
                index2 = 2 * mynode + 1 + pow(2, i); //2 + 1 + 2 = 5
                

                alpha = A[1][index1] / A[3][index1];
                gamma = A[1][index2] / A[4][index2];

                if (mynode == (numactivep - 1) && numactivep % 2 == 0) {
                printf("nodex: %d A[0][0]: %e A[0][1]: %e A[0][2]: %e A[0][3]: %e A[0][4]: %e\n", mynode, A[0][0], A[0][1], A[0][2], A[0][3], A[0][4]);
                printf("nodex: %d A[1][0]: %e A[1][1]: %e A[1][2]: %e A[1][3]: %e A[0][4]: %e\n", mynode, A[1][0], A[1][1], A[1][2], A[1][3], A[1][4]);
                printf("nodex: %d A[2][0]: %e A[2][1]: %e A[2][2]: %e A[2][3]: %e A[0][4]: %e\n", mynode, A[2][0], A[2][1], A[2][2], A[2][3], A[2][4]);
                printf("nodex: %d A[3][0]: %e A[3][1]: %e A[3][2]: %e A[3][3]: %e A[0][4]: %e\n", mynode, A[3][0], A[3][1], A[3][2], A[3][3], A[3][4]);
                printf("nodex: %d A[4][0]: %e A[4][1]: %e A[4][2]: %e A[4][3]: %e A[0][4]: %e\n", mynode, A[4][0], A[4][1], A[4][2], A[4][3], A[4][4]);
                    index1 = j + 1;
                    printf("QWE i: %d j: %d numactivep: %d\n", i, j, numactivep);
                    index2 = index1 + 2;
                    alpha = A[1][index1] / A[3][index1];
                    for (k = 0; k < size + 1; k++) {
                        A[1][k] -= (alpha * A[3][k]);
                    }
                } else {
                    for (k = 0; k < size + 1; k++) {
                        A[1][k] -= (alpha * A[3][k] + gamma * A[4][k]);
                    }
                }
                
                if (numactivep > 1) {
                    if (j == 0) {
                        MPI_Send(A[1], size + 1, MPI_DOUBLE, activep[1], 0, MPI_COMM_WORLD);
                        printf("sent\n");
                    } else if (j == numactivep - 1 && numactivep % 2 == 0) {
                        MPI_Recv(A[3], size + 1, MPI_DOUBLE, activep[j - 1], 0, MPI_COMM_WORLD, &status);
                        printf("qweeee\n");
                    } else if (j == numactivep -1) {
                        MPI_Send(A[1], size + 1, MPI_DOUBLE, activep[numactivep - 2], 1, MPI_COMM_WORLD);
                    } else if (j % 2 == 0) {
                        MPI_Send(A[1], size + 1, MPI_DOUBLE, activep[j - 1], 1, MPI_COMM_WORLD);
                        MPI_Send(A[1], size + 1, MPI_DOUBLE, activep[j + 1], 0, MPI_COMM_WORLD);
                    } else {
                        printf("qwe");
                        MPI_Recv(A[3], size + 1, MPI_DOUBLE, activep[j - 1], 0, MPI_COMM_WORLD, &status);
                        MPI_Recv(A[4], size + 1, MPI_DOUBLE, activep[j + 1], 1, MPI_COMM_WORLD, &status);
                    }
                }
            }
        }

        numactivep = 0;
        for (j = activep[1]; j < totalnodes; j = j + pow(2, i + 1)) {
            activep[numactivep++] = j;
        }
        lastProcess = activep[numactivep - 1];
    }

    double x[totalnodes];
    for (j = 0; j < totalnodes; j++) {
        x[j] = 0.0;
    }

    if (mynode == activep[0]) {
        printf("node: %d\n", mynode);
                printf("node: %d A[0][0]: %e A[0][1]: %e A[0][2]: %e A[0][3]: %e A[0][4]: %e\n", mynode, A[0][0], A[0][1], A[0][2], A[0][3], A[0][4]);
                printf("node: %d A[1][0]: %e A[1][1]: %e A[1][2]: %e A[1][3]: %e A[0][4]: %e\n", mynode, A[1][0], A[1][1], A[1][2], A[1][3], A[1][4]);
                printf("node: %d A[2][0]: %e A[2][1]: %e A[2][2]: %e A[2][3]: %e A[0][4]: %e\n", mynode, A[2][0], A[2][1], A[2][2], A[2][3], A[2][4]);
                printf("node: %d A[3][0]: %e A[3][1]: %e A[3][2]: %e A[3][3]: %e A[0][4]: %e\n", mynode, A[3][0], A[3][1], A[3][2], A[3][3], A[3][4]);
                printf("node: %d A[4][0]: %e A[4][1]: %e A[4][2]: %e A[4][3]: %e A[0][4]: %e\n", mynode, A[4][0], A[4][1], A[4][2], A[4][3], A[4][4]);

        x[mynode] = A[1][size] / A[1][(size - 1) ];
    }

    double tmp;

    for (i = log2(size + 1) - 3; i >= 0 ; i--) {
        printf("III : %d\n", i);
        tmp = x[mynode];
        MPI_Allgather(&tmp, 1, MPI_DOUBLE, x, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        numactivep = 0;
        for (j = activep[0] - pow(2, i); j < totalnodes; j = j + pow(2, i + 1)) {
            activep[numactivep++] = j;
        }

        for (j = 0; j < numactivep; j++) {
            if (mynode == activep[j]) {
                x[mynode] = A[1][size];
                for (k = 0; k < totalnodes; k++) {
                    if (k != mynode) {
                        x[mynode] -= A[1][2 * k + 1] * x[k];
                    }
                }
                x[mynode] = x[mynode] / A[1][2 * mynode + 1];
            }
        }
    }

    tmp = x[mynode];
    MPI_Allgather(&tmp, 1, MPI_DOUBLE, x, 1, MPI_DOUBLE, MPI_COMM_WORLD);

    for (k = 0; k < totalnodes; k++) {
        A[0][size] -= A[0][2 * k + 1] * x[k];
        A[2][size] -= A[2][2 * k + 1] * x[k];
    }

    A[0][size] = A[0][size] / A[0][2 * mynode];
    A[1][size] = x[mynode];
    A[2][size] = A[2][size] / A[2][2 * mynode + 2];

    for (i = 0; i < 3; i++) {
        printf("x%d = %e\n", 2 * mynode + i, A[i][size]);
    }
    
    MPI_Finalize();
    return 0;
}
