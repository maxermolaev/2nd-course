#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 5000
#define EPSILON 1E-7
#define TAU 1E-5
#define MAX_ITERATION_COUNT 1000000

// void SplitMatrixLines(int* line_counts, int* line_offsets, int matrix_size, int thread_count) {
//     int offset = 0;
//     for (int i = 0; i < thread_count; ++i) {
//         line_counts[i] = matrix_size / thread_count;
//         if (i < matrix_size % thread_count) {
//             ++line_counts[i];
//         }
//         line_offsets[i] = offset;
//         offset += line_counts[i];
//     }
// }

void FillMatrixA(double* A, int matrix_size) {
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; ++j) {
            A[i * matrix_size + j] = 1;
        }
        A[i * matrix_size + i] = 2;
    }
}

void FillVectorX(double* x, int vector_size) {
    for (int i = 0; i < vector_size; i++) {
        x[i] = 0;
    }
}

void FillVectorB(double* b, int vector_size) {
    for (int i = 0; i < vector_size; i++) {
        b[i] = N + 1;
    }
}

double CalculateNormSquare(const double* vector, int vector_size) {
    double norm_square = 0.0;
    for (int i = 0; i < vector_size; ++i) {
        norm_square += vector[i] * vector[i];
    }
    return norm_square;
}

void CalculateAxb(const double* A, const double* x, const double* b, double* Axb, int size) {
#pragma omp for
    for (int i = 0; i < size; ++i) {
        Axb[i] = -b[i];
        for (int j = 0; j < N; ++j) {
            Axb[i] += A[i * N + j] * x[j];
        }
    }
}

void CalculateNextX(const double* Axb, double* x, double tau, int vector_size) {
#pragma omp for
    for (int i = 0; i < vector_size; ++i) {
        x[i] -= tau * Axb[i];
    }
}

void PrintMatrix(const double* a, int lines, int columns) {
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%lf ", a[i * columns + j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    int iteration_count = 0;
    int thread_id;
    int thread_count = omp_get_max_threads();
    double accuracy = EPSILON + 1;
    double b_norm;
    double start_time;
    double end_time;
    int* line_counts = malloc(sizeof(int) * thread_count);
    //int* line_offsets = malloc(sizeof(int) * thread_count);
    double* A = malloc(sizeof(double) * N * N);
    double* x = malloc(sizeof(double) * N);
    double* b = malloc(sizeof(double) * N);
    double* Axb = malloc(sizeof(double) * N);
    
    //SplitMatrixLines(line_counts, line_offsets, N, thread_count);
    
    FillMatrixA(A, N);
    FillVectorX(x, N);
    FillVectorB(b, N);
    
    b_norm = sqrt(CalculateNormSquare(b, N));
    
    start_time = omp_get_wtime();
    
#pragma omp parallel
{
    int local_done = 0;
    double local_accuracy;
    int local_iteration_count = 0;

    while (!local_done) {
        CalculateAxb(A, x, b, Axb, N);
        CalculateNextX(Axb, x, TAU, N);

        local_accuracy = sqrt(CalculateNormSquare(Axb, N)) / b_norm;
        
        #pragma omp critical //только один поток выполняет 
        {
            if (local_accuracy < accuracy) {
                accuracy = local_accuracy;
            }
            local_iteration_count++;
            if (local_accuracy <= EPSILON || local_iteration_count >= MAX_ITERATION_COUNT) {
                local_done = 1;
            }
        }
    }

    #pragma omp atomic //для одной простой операции
    iteration_count += local_iteration_count;
}
    
    end_time = omp_get_wtime();
    
    if (iteration_count == MAX_ITERATION_COUNT) {
        printf("Too many iterations\n");
    } else {
        //printf("Matrix: \n");
        //PrintMatrix(x, 1, 10);
        printf("Norm: %lf\n", sqrt(CalculateNormSquare(x, N)));
        printf("Time: %lf sec\n", end_time - start_time);
    }
    
    free(A);
    free(x);
    free(b);
    free(Axb);
    
    return 0;
}

