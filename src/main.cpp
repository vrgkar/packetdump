#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <unistd.h>

/* Function to print Ethernet header */
void print_ethernet_header(const unsigned char *buffer) {
    struct ethhdr *eth = (struct ethhdr *)buffer;
    printf("\nEthernet Header\n");
    printf("   |-Source Address        : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    printf("   |-Destination Address   : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    printf("   |-Protocol              : %u\n", (unsigned short)eth->h_proto);
}

/* Function to print IP header */
void print_ip_header(const unsigned char *buffer) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in source, dest;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    printf("\nIP Header\n");
    printf("   |-Source IP        : %s\n", inet_ntoa(source.sin_addr));
    printf("   |-Destination IP   : %s\n", inet_ntoa(dest.sin_addr));
    printf("   |-Protocol         : %d\n", (unsigned int)iph->protocol);
    printf("   |-TTL              : %d\n", (unsigned int)iph->ttl);
    printf("   |-IP Total Length  : %d\n", ntohs(iph->tot_len));
    printf("   |-Identification   : %d\n", ntohs(iph->id));
}

/* Function to print TCP header */
void print_tcp_header(const unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    int ip_header_len = iph->ihl * 4;
    struct tcphdr *tcph = (struct tcphdr *)(buffer + ip_header_len + sizeof(struct ethhdr));

    printf("\nTCP Header\n");
    printf("   |-Source Port      : %u\n", ntohs(tcph->source));
    printf("   |-Destination Port : %u\n", ntohs(tcph->dest));
    printf("   |-Sequence Number    : %u\n", ntohl(tcph->seq));
    printf("   |-Acknowledge Number : %u\n", ntohl(tcph->ack_seq));
    printf("   |-Header Length      : %d\n", (unsigned int)tcph->doff * 4);
    printf("   |-Urgent Flag          : %d\n", (unsigned int)tcph->urg);
    printf("   |-Acknowledgement Flag : %d\n", (unsigned int)tcph->ack);
    printf("   |-Push Flag            : %d\n", (unsigned int)tcph->psh);
    printf("   |-Reset Flag           : %d\n", (unsigned int)tcph->rst);
    printf("   |-Synchronise Flag     : %d\n", (unsigned int)tcph->syn);
    printf("   |-Finish Flag          : %d\n", (unsigned int)tcph->fin);
    printf("   |-Window         : %d\n", ntohs(tcph->window));
    printf("   |-Checksum       : %d\n", ntohs(tcph->check));
    printf("   |-Urgent Pointer : %d\n", tcph->urg_ptr);
}

/* Function to process and print packet details */
void process_packet(const unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));

    print_ethernet_header(buffer);
    print_ip_header(buffer);

    switch (iph->protocol) {
        case 6:  // TCP Protocol
            print_tcp_header(buffer, size);
            break;
        default:
            printf("Other protocol: %d\n", iph->protocol);
            break;
    }
}

int main() {
    int raw_sock;
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);
    unsigned char *buffer = (unsigned char *)malloc(65536);

    /* Create a raw socket */
    raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_sock < 0) {
        perror("Socket Error");
        return 1;
    }

    while (1) {
        /* Receive a packet */
        int data_size = recvfrom(raw_sock, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_len);
        if (data_size < 0) {
            perror("Recvfrom Error");
            return 1;
        }
        /* Process the packet */
        process_packet(buffer, data_size);
    }

    close(raw_sock);
    free(buffer);

    return 0;
}
