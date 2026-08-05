#include "Client.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;
// the port
#define PORT 3490
// the size of the buffer
#define BUF_SIZE 200

Client::Client()
{
}

void Client::run()
{
    char buf[BUF_SIZE];
    size_t nbytes = BUF_SIZE - 1;
    int sockfd;
    // connector addr
    struct sockaddr_in their_addr;
    if ((sockfd = socket (PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("socket");
        exit (1);
    }
    // interp’d by host
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons (PORT);
    their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // zero rest of struct
    bzero (&(their_addr.sin_zero), 8);
    if (connect (sockfd, (struct sockaddr*)&their_addr,
        sizeof (struct sockaddr)) == -1)
    {
        perror ("connect");
        exit (1);
    }
    string str;
    cout << "Command: ";
    // read the entire line as it may contain spaces
    getline(cin, str);
    while (1)
    {
        if (str == "exit")
        {
            send (sockfd, str.c_str(), str.length() + 1, 0);
            break;
        }
        write (sockfd, str.c_str(), str.length() + 1);
        int r = recv (sockfd, buf, nbytes, 0);
        if ((r > 0) && (strlen(buf) > 0))
        {
            cout << buf << endl;
        }
        cout << "Command: ";
        // read the entire line as it may contain spaces
        getline(cin, str);
    }
    close(sockfd);
}

int main(int argc, char* argv[])
{
    Client client;
    client.run();
    return 0;
}

