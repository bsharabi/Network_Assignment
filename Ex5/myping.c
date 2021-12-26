#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#define SOURCE_IP "localhost"
#define DESTINATION_IP "8.8.8.8" // google Dns
#define PING_SLEEP_RATE 1000000
#define PCKT_LEN 1024

// ICMP header len for echo req
#define ICMP_HDRLEN 8
// Checksum algo
unsigned short calculate_checksum(unsigned short *paddress, int len);
// 1. Change SOURCE_IP and DESTINATION_IP to the relevant
//     for your computer
// 2. Compile it using MSVC compiler or g++
// 3. Run it from the account with administrative permissions,
//    since opening of a raw-socket requires elevated preveledges.
//
//    On Windows, right click the exe and select "Run as administrator"
//    On Linux, run it as a root or with sudo.
//
// 4. For debugging and development, run MS Visual Studio (MSVS) as admin by
//    right-clicking at the icon of MSVS and selecting from the right-click
//    menu "Run as administrator"
//
//  Note. You can place another IP-source address that does not belong to your
//  computer (IP-spoofing), i.e. just another IP from your subnet, and the ICMP
//  still be sent, but do not expect to see ICMP_ECHO_REPLY in most such cases
//  since anti-spoofing is wide-spread.

// Compute checksum (RFC 1071).
unsigned short calculate_checksum(unsigned short *paddress, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = paddress;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *((unsigned char *)&answer) = *((unsigned char *)w);
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

int main()
{
    struct icmp icmphdr; // ICMP-header
    char data[IP_MAXPACKET] = "Sending ping, This is the ping.\n";
    int datalen = strlen(data) + 1;
    // Message Type (8 bits): ICMP_ECHO_REQUEST
    icmphdr.icmp_type = ICMP_ECHO;
    // Message Code (8 bits): echo request
    icmphdr.icmp_code = 0;
    // Identifier (16 bits): some number to trace the response.
    // It will be copied to the response packet and used to map response to the request sent earlier.
    // Thus, it serves as a Transaction-ID when we need to make "ping"
    icmphdr.icmp_id = 101; // Michael
    // Sequence Number (16 bits): starts at 0
    icmphdr.icmp_seq = 0;
    // ICMP header checksum (16 bits): set to 0 not to include into checksum calculation
    icmphdr.icmp_cksum = 0;
    // Combine the packet
    char packet[IP_MAXPACKET];
    // First, IP header.
    memcpy((packet), &icmphdr, ICMP_HDRLEN);
    // Next, ICMP header
    memcpy(packet + ICMP_HDRLEN, data, datalen);
    // Calculate the ICMP header checksum
    icmphdr.icmp_cksum = calculate_checksum((unsigned short *)(packet), ICMP_HDRLEN + datalen);
    memcpy((packet), &icmphdr, ICMP_HDRLEN);
    /* Structure describing an Internet socket address.  */
    struct sockaddr_in dest_in;
    /* Set N bytes of S to C.  */
    memset(&dest_in, 0, sizeof(struct sockaddr_in));
    /* IP protocol family.  */
    dest_in.sin_family = AF_INET;

    /* Convert from presentation format of an Internet number in buffer
       starting at CP to the binary network format and store result for
       interface type AF in buffer starting at BUF.  */
    inet_pton(AF_INET, DESTINATION_IP, &(dest_in.sin_addr));

    int sock = -1;
    /* Create a new socket of type TYPE in domain DOMAIN, using
    protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
    Returns a file descriptor for the new socket, or -1 for errors.  */

    // AF_INET --> IP protocol family.
    // SOCK_RAW --> Reliably-delivered messages.
    // IPPROTO_ICMP --> Internet Group Management Protocol.
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        fprintf(stderr, "socket() failed with error: %d", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        return -1;
    }
    int ttl_val = 64;
    struct timeval tv_out;
    if (setsockopt(sock, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0)
    {
        printf("\nSetting socket options to TTL failed!\n");
        return;
    }
    else
    {
        printf("\nSocket set to TTL..\n");
    }

    // setting timeout of recv setting
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_out, sizeof tv_out);

    /* A time value that is accurate to the nearest
        microsecond but also has a range of years.  */
    // tv_sec -->  Seconds.
    // tv_usec -->  Microseconds.
    struct timeval start, stop;
    long double rtt_msec = 0;
    while (1)
    {

        // Get time of send
        gettimeofday(&start, NULL);
        // Send the packet using sendto() for sending datagrams.
        /* Send N bytes of BUF on socket FD to peer at address ADDR (which is
            ADDR_LEN bytes long).  Returns the number sent, or -1 for errors.
            This function is a cancellation point and therefore not marked with
             __THROW.  */
        if (sendto(sock, packet, ICMP_HDRLEN + datalen, 0, (struct sockaddr *)&dest_in, sizeof(dest_in)) == -1)
        {
            fprintf(stderr, "sendto() failed with error: %d", errno);
        }

        char buff[PCKT_LEN];
        /* Set N bytes of S to 0.  */
        bzero(buff, sizeof(buff));

        int length = sizeof(dest_in);
        int bytes = recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr *)&dest_in, &length);
        if (bytes == -1)
        {
            fprintf(stderr, "recvfrom() failed with error: %d", errno);
        }
        /*Get the current time of day, putting it into *TV.
        If TZ is not null, *TZ must be a struct timezone, and both fields
        will be set to zero.
        Calling this function with a non-null TZ is obsolete;
        use localtime etc. instead.
        This function itself is semi-obsolete;
        most callers should use time or clock_gettime instead.*/
        gettimeofday(&stop, NULL);
        char buffer[INET_ADDRSTRLEN];
        /* Convert a Internet address in binary network format for interface
        type AF in buffer starting at CP to presentation form and place
        result in buffer of length LEN astarting at BUF.  */
        inet_ntop(AF_INET, &dest_in.sin_addr, buffer, sizeof(buffer));
        double timeElapsed = ((double)(stop.tv_usec - start.tv_usec)) / 1000000.0;
        rtt_msec = (stop.tv_sec - start.tv_sec) * 1000.0 + timeElapsed;

        printf("%d bytes from %s icmp_seq=%d ttl=%d rtt=%Lf ms\n", bytes, buffer, ++icmphdr.icmp_seq, ttl_val, rtt_msec);
        usleep(PING_SLEEP_RATE);
    }
    close(sock);

    return 0;
}