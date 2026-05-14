#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 9999
#define BUF_SIZE 1024

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind"); exit(1);
    }
    listen(server_fd, 1);
    printf("TCP服务器已启动，等待连接...\n");

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) { perror("accept"); exit(1); }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    FILE *fp = fopen("received_tcp.dat", "wb");
    char buffer[BUF_SIZE];
    ssize_t recv_len;
    while ((recv_len = read(client_fd, buffer, BUF_SIZE)) > 0) {
        fwrite(buffer, 1, recv_len, fp);
    }

    gettimeofday(&end, NULL);
    double time_used = (end.tv_sec - start.tv_sec) + 
                       (end.tv_usec - start.tv_usec)/1000000.0;
    fclose(fp); close(client_fd); close(server_fd);

    printf("TCP传输完成，耗时: %.2fs，速率: %.2f MB/s\n", 
           time_used, 200/(1024*time_used));
    return 0;
}
