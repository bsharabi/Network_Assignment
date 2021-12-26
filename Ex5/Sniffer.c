#include <netinet/in.h>
#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define IP_RF 0x0800 /* reserved fragment flag */
#define ETHER_ADDR_LEN 6

typedef struct sniff_ethernet
{
  u_char ether_dhost[ETHER_ADDR_LEN]; /* destination host address */
  u_char ether_shost[ETHER_ADDR_LEN]; /* source host address */
  u_short ether_type;                 /* IP? ARP? RARP? etc */
} ethheader;
typedef struct sniff_ip
{
  unsigned char ip_vhl : 4, iph_ver : 4;       /* version << 4 | header length >> 2 */
  unsigned char ip_tos;                        /* type of service */
  unsigned short int ip_len;                   /* total length */
  unsigned short int ip_id;                    /* identification */
  unsigned short int ip_off : 13, ip_flag : 3; /* fragment offset field */
  unsigned char ip_ttl;                        /* time to live */
  unsigned char ip_p;                          /* protocol */
  unsigned short int ip_sum;                   /* checksum */
  struct in_addr ip_src, ip_dst;               /* source and dest address */
} ipheader;

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

int main()
{
  pcap_t *handle;
  char errbuf[PCAP_ERRBUF_SIZE];

  struct bpf_program fp;
  char filter_exp[] = "ip proto ICMP";
  bpf_u_int32 net;

  if ((handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf)) == NULL)
  {
    printf("Can't open eth0: %s\n", errbuf);
    exit(1);
  }

  int pcaperr;
  if ((pcaperr = pcap_compile(handle, &fp, filter_exp, 0, net)) == -1)
    printf("%s\n", pcap_geterr(handle));

  pcap_setfilter(handle, &fp);

  pcap_loop(handle, -1, got_packet, NULL);

  pcap_close(handle); 
  return 0;
}
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)

{
  ethheader *eth = (ethheader *)packet;

  if (ntohs(eth->ether_type) == IP_RF)
  {
    
    ipheader *ip = (ipheader *)(packet + sizeof(ethheader));
    printf("\n-----ICMP-----\n");
    printf("From: %s\n", inet_ntoa(ip->ip_src));
    printf("To: %s\n", inet_ntoa(ip->ip_dst));
    printf("--------------");
    return;
  }
}