#include <stdio.h>
#include <time.h>
// вычисляем e^x с помощью разложения в ряд Маклорена
double exponent(double x, int N) {
    double sum = 1.0;
    double term;
    
    for (int n = 1; n < N; n++) {
        term = 1;
        for (int i = 1; i <= n; i++) {
            term *= x / i;
        }
        sum += term;
    }
    return sum;
}

int main() {
    double x;
    int N;
    if(scanf("%lf %d", &x,&N)) { 
        double result = exponent(x, N);
        printf("%.2f %d %.10f\n", x, N, result);
    }
    return 0;
}
