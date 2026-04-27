#include <iostream>
#include <cstdlib>
#include <cstring>
#include <complex> 

void print_usage(const char *prog_name)
{
    fprintf(stderr, "Usage: %s [real_part] [imaginary_part] [iterations]\n", prog_name);
    fprintf(stderr, "Check whether a complex number belongs to the Mandelbrot set.\n\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "    real_part        Real part of the complex number\n");
    fprintf(stderr, "    imaginary_part   Imaginary part of the complex number\n");
    fprintf(stderr, "    iterations       Optional. Number of iterations (default: 15)\n");
}

int main(int argc, char *argv[])
{
    std::complex<double> ans(0, 0);
    int N = 15;
    if (argc > 4 || argc < 3) 
    {
        print_usage(argv[0]);
        return 1;
    }
    if (argc == 4)
    {
        N = atoi(argv[3]); 
    }
    double real_part = atof(argv[1]);
    double imaginary_part = atof(argv[2]);
    if (real_part == 0 && imaginary_part == 0)
    {
        exit(0);
    }
    std::complex<double> z(real_part, imaginary_part);
    for (int i = 0; i < N; i++){
        ans = ans*ans +z;
        if (std::abs(ans) > 10000000)
        {
            printf("%.2f + %.2fi is NOT in the Mandelbrot set \n", real_part, imaginary_part);
            return 1;
        }
    }
    printf("%.2f + %.2fi is in the Mandelbrot set\n", real_part, imaginary_part);
    return 0;
}