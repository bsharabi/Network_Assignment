#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

void sendSocket(int socket)
{
    char *fname;
    fname = "1gb.txt";
    FILE *MyFile;
    MyFile = fopen(fname, "r");
    char data[1024] = {0};
    while (fgets(data, 1024, MyFile))
    {
        send(socket, data, 1024, 0);
        printf("%s", data);
        bzero(data, 1024);
    }
}
int main()
{
   
    int i = 0;
    do
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = 8080;
        sa.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect(sock, (struct sockaddr *)&sa, sizeof(sa));
        sendSocket(sock);
        close(sock);
    } while (++i < 5);

    i = 0;
    do
    {
        int sock2 = socket(AF_INET, SOCK_STREAM, 0);
        char buf[256];
        socklen_t len;
        strcpy(buf, "reno");
        len = sizeof(buf);
        if (setsockopt(sock2, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
        {
            perror("setsockopt");
            return -1;
        }

        struct sockaddr_in sa2;
        sa2.sin_family = AF_INET;
        sa2.sin_port = 8080;
        sa2.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect(sock2, (struct sockaddr *)&sa2, sizeof(sa2));
        sendSocket(sock2);
        close(sock2);
    } while (++i < 5);

    return 0;
}
