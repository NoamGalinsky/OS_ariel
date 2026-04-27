#include <iostream>
#include <csignal>
#include <unistd.h>

volatile sig_atomic_t current_number = 0;
volatile sig_atomic_t bit_count = 0;

void handle_signal(int sig) {
    current_number <<= 1;  // shift left to make room for new bit
    if (sig == SIGUSR2) {
        current_number |= 1;
    }
    bit_count++;

    if (bit_count == 8) {
        std::cout << "Received: " << current_number << std::endl;
        exit(0);
    }
}

int main() {
    std::cout << "My PID is: " << getpid() << std::endl;

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);
    sigaction(SIGUSR1, &sa, nullptr);
    sigaction(SIGUSR2, &sa, nullptr);

    while (true) {
        pause(); // wait for signals
    }

    return 0;
}
