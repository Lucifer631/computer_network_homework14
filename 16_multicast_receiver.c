#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>

#define MULTICAST_GROUP "239.255.0.1"
#define PORT 8888
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *output_file = argv[1];

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    // 绑定端口
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind"); exit(1);
    }

    // 加入多播组
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt"); exit(1);
    }

    FILE *fp = fopen(output_file, "wb");
    if (!fp) { perror("fopen"); exit(1); }

    char buffer[BUF_SIZE];
    ssize_t recv_len;
    printf("等待接收文件...\n");
    while ((recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL)) > 0) {
        if (strncmp(buffer, "EOF", 3) == 0) break;
        fwrite(buffer, 1, recv_len, fp);
    }

    printf("文件接收完成，已保存为: %s\n", output_file);
    fclose(fp);
    close(sockfd);
    return 0;
}
