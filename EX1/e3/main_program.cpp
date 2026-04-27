#include "Mandelbrot.h"
#include <iostream>



int main()
{
    double real, imag;
    std::complex<double> c;
    printf("Write the real and imag number: \n");
    scanf("%lf %lf", &real, &imag);
    if (real == 0 && imag == 0)
    {
        exit(0);
    }
    c.real(real);
    c.imag(imag);
    is_in_mandelbrot(c ,20);
    return 0; 
}