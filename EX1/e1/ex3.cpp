int main()
{
    int *ptr = (int *)0xDEADBEEF;
    *ptr  = 6;
    return 0;    
}