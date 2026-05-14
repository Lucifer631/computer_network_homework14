#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8889
#define BUF_SIZE 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind"); exit(1);
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    FILE *fp = fopen("received_udp.dat", "wb");
    char buffer[BUF_SIZE];
    socklen_t addr_len = sizeof(server_addr);
    ssize_t recv_len;
    while ((recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, 
                               (struct sockaddr*)&server_addr, &addr_len)) > 0) {
        if (strncmp(buffer, "EOF", 3) == 0) break;
        fwrite(buffer, 1, recv_len, fp);
    }

    gettimeofday(&end, NULL);
    double time_used = (end.tv_sec - start.tv_sec) + 
                       (end.tv_usec - start.tv_usec)/1000000.0;
    fclose(fp); close(sockfd);

    printf("UDP传输完成，耗时: %.2fs，速率: %.2f MB/s\n", 
           time_used, 200/(1024*time_used));
    return 0;
}
