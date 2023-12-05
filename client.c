// client.c
#include "func.c"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 12345

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main() {
    int client_socket, n;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char buffer[256];

    // 클라이언트 소켓 생성
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        error("ERROR opening socket");
    }

    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    // 서버에 연결 요청
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
    }

    // 서버로 메시지 전송
    n = write(client_socket, "Hello, server!", strlen("Hello, server!"));
    if (n < 0) {
        perror("socket");
    }

    printf("서버에 연결되었습니다.\n상대 플레이어를 기다리는중...\n");

    // 서버로부터 게임 시작 메시지 수신
    memset(buffer, 0, sizeof(buffer));
    n = read(client_socket, buffer, sizeof(buffer));
    if (n < 0) {
        perror("socket");
    }
    printf("%s\n", buffer);

    // 소켓 닫기
    close(client_socket);

    return 0;
}
