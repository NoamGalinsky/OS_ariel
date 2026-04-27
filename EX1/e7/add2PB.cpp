#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static ssize_t write_str(int fd, const char *s) {
    ssize_t len = 0;
    while (s[len]) len++;
    return write(fd, s, len);
}

/*
 * Usage:
 *   add2PB "First Last" phone         (argc == 3)
 *   add2PB First Last phone           (argc == 4)
 *
 * Appends a line to phonebook.txt in the format:
 *   Name,phone\n
 */
int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        write(STDERR_FILENO, "Usage: add2PB <Name> <Phone>\n"
                             "       add2PB <FirstName> <LastName> <Phone>\n", 74);
        return 1;
    }

    int fd = open("phonebook.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        write(STDERR_FILENO, "Error: cannot open phonebook.txt\n", 33);
        return 1;
    }

    if (argc == 3) {
        /* add2PB "First Last" phone */
        write_str(fd, argv[1]);
        write(fd, ",", 1);
        write_str(fd, argv[2]);
    } else {
        /* add2PB First Last phone */
        write_str(fd, argv[1]);
        write(fd, " ", 1);
        write_str(fd, argv[2]);
        write(fd, ",", 1);
        write_str(fd, argv[3]);
    }
    write(fd, "\n", 1);

    close(fd);
    return 0;
}
