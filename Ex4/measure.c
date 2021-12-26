#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

void recieveSocket(int socket)
{
    struct sockaddr_in na;
    socklen_t nasize = sizeof(na);
    int rsocket = accept(socket, (struct sockaddr *)&na, &nasize);
    char data[1024] = {0};
    int receive = 1;
    while (receive != 0 && receive != -1)
    {
        receive = recv(rsocket, data, 1024, 0);
        bzero(data, 1024);
    }
}

int main()
{
    printf("connect server\n");
    struct timeval begin, end;
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = 8080;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int b = bind(sock, (struct sockaddr *)&sa, sizeof(sa));
    int l = listen(sock, 10);
    int i = 0;
    do
    {
        gettimeofday(&begin, 0);
        recieveSocket(sock);
        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds * 1e-6;
        printf("Time measured: %.3f seconds (Cubic).\n", elapsed);
    } while (++i < 5);

    // reno changing
    printf("From CUBIC to RENO\n");
    struct timeval begin2, end2;
    char buf[256];
    socklen_t len;
    strcpy(buf, "reno");
    len = sizeof(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
    {
        perror("setsockopt");
        return -1;
    }

    i = 0;
    do
    {
        gettimeofday(&begin2, 0);
        recieveSocket(sock);
        gettimeofday(&end2, 0);
        long seconds2 = end2.tv_sec - begin2.tv_sec;
        long microseconds2 = end2.tv_usec - begin2.tv_usec;
        double elapsed2 = seconds2 + microseconds2 * 1e-6;
        printf("Time measured: %.3f seconds (Reno).\n", elapsed2);
    } while (++i < 5);
    return 0;
}
