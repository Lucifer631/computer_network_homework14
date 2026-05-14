#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 9999
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *server_ip = argv[1];
    const char *file_path = argv[2];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect"); exit(1);
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    FILE *fp = fopen(file_path, "rb");
    char buffer[BUF_SIZE];
    size_t read_len;
    while ((read_len = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        write(sockfd, buffer, read_len);
    }

    gettimeofday(&end, NULL);
    double time_used = (end.tv_sec - start.tv_sec) + 
                       (end.tv_usec - start.tv_usec)/1000000.0;
    fclose(fp); close(sockfd);

    printf("TCP传输完成，耗时: %.2fs，速率: %.2f MB/s\n", 
           time_used, 200/(1024*time_used));
    return 0;
}
