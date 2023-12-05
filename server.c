// server.c
#include "func.c"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 12345
#define MAX_CLI 3 //최대 세 명의 클라이언트 허용

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int serv_sock, clie_sock[MAX_CLI];
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    int n, num_clients;

    char buffer[256];

    // 서버 소켓 생성
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0) {
        perror("socket");
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 소켓과 주소 바인딩
    if (bind(serv_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // 연결 요청 대기
    listen(serv_sock, 5);
    printf("Server Start. Waiting for Clients...\n");

    client_len = sizeof(client_addr);

    // 두 명의 클라이언트 연결 수락
    while (num_clients < MAX_CLI) {
        clie_sock[num_clients] = accept(serv_sock, (struct sockaddr *)&client_addr, &client_len);
        if (clie_sock[num_clients] < 0) {
            perror("accept");
            exit(1);
        }
        printf("클라이언트 %d 접속.\n", num_clients + 1);
        num_clients++;

        // 두 명의 클라이언트가 연결되면 게임 시작 메시지 전송
        if (num_clients == MAX_CLI) {
            for (int i = 0; i < MAX_CLI; i++) {
                write(clie_sock[i], "게임을 시작합니다.", strlen("게임을 시작합니다."));
            }
            printf("게임을 시작합니다.\n");
        }
    }

    // 클라이언트로부터 메시지 수신
    memset(buffer, 0, sizeof(buffer));
    n = read(clie_sock, buffer, sizeof(buffer));
    if (n < 0) {
        perror("socket");
        exit(1);
    }
    printf("Message from Client: %s\n", buffer);

    // 클라이언트로 메시지 전송
    n = write(clie_sock, "메시지 받음!", strlen("메시지 받음!"));
    if (n < 0) {
        perror("socket");
        exit(1);
    }

    // 소켓 닫기
    close(clie_sock);
    close(serv_sock);

    return 0;
}
