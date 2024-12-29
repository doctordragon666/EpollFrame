#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    // 创建套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    // 连接到远程服务器
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        close(sock);
        return 1;
    }

    
    // 接收回复
    printf("Waiting for server reply...");
    fflush(stdout);
    if (recv(sock, server_reply, BUFFER_SIZE, 0) < 0) {
        perror("recv failed");
    } else {
        printf("Server reply : %s", server_reply);
    }

    // 发送数据
    printf("Enter message to send : ");
    fgets(message, BUFFER_SIZE, stdin);
    if (send(sock, message, strlen(message), 0) < 0) {
        perror("Send failed");
        close(sock);
        return 1;
    }


    close(sock);
    return 0;
}