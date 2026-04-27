#include <iostream>
#include <csignal>
#include <unistd.h>

void send_bit(pid_t pid, int bit) {
    if (bit == 0)
        kill(pid, SIGUSR1);
    else
        kill(pid, SIGUSR2);

    usleep(100000); // small delay for receiver
}

int main()
{
    int receiver_pid;
    int myNumber;
    std::cout << "Enter received PID: " ;
    std::cin >> receiver_pid;

    std::cout << "Enter Message: " ;
    std::cin >> myNumber;
    for (int i = 7; i >= 0; --i) { // send MSB first
        int bit = (myNumber >> i) & 1;
        send_bit(receiver_pid, bit);
    }
    return 0;
}
