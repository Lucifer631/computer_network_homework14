#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_GROUP "239.255.0.1"
#define PORT 8888
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *file_path = argv[1];

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    struct sockaddr_in multicast_addr;
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_addr.sin_port = htons(PORT);

    FILE *fp = fopen(file_path, "rb");
    if (!fp) { perror("fopen"); exit(1); }

    char buffer[BUF_SIZE];
    size_t read_len;
    printf("开始发送文件: %s\n", file_path);
    while ((read_len = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        sendto(sockfd, buffer, read_len, 0, 
               (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
    }
    // 发送结束标记
    strcpy(buffer, "EOF");
    sendto(sockfd, buffer, 3, 0, 
           (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));

    printf("文件发送完成\n");
    fclose(fp);
    close(sockfd);
    return 0;
}
