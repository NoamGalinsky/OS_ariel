
#include "Mandelbrot.h"

bool is_in_mandelbrot(std::complex<double> c, int N)
{
    std::complex<double> ans(0, 0);
    for (int i = 0; i < N; i++){
        ans = ans*ans +c;
        if (std::abs(ans) > 10000000)
        {
            printf("%.2f + %.2fi is NOT in the Mandelbrot set \n", c.real(), c.imag());
            return false;
        }
    }
    printf("%.2f + %.2fi is in the Mandelbrot set\n", c.real(), c.imag());
    return true;
}
