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
    int n, idx = 1, num_clients = 0;
    // 각 클라이언트에게 할당해줄 플레이어 구조체 선언
    struct Player players[MAX_CLI];
    // 덱 생성
    struct Card card_all[52];

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

    // 최대 클라이언트 수 접속까지 연결 요청 대기
    listen(serv_sock, MAX_CLI);
    printf("서버 가동 시작.\n클라이언트 접속 대기...\n");

    client_len = sizeof(client_addr);

    // 게임 루프
    while (1) {
        // 클라이언트 연결 수락 및 플레이어 데이터 초기화
        if (num_clients < MAX_CLI) {
            clie_sock[num_clients] = accept(serv_sock, (struct sockaddr *)&client_addr, &client_len);
            // 플레이어 데이터 초기화
            struct Player newPlayer;
            reset(&newPlayer); // resetPlayers() 함수 호출로 플레이어 데이터 초기화
            players[num_clients] = newPlayer; // 초기화된 데이터를 서버의 플레이어 배열에 할당
            newPlayer.idx += (idx);// 플레이어 번호 부여

            //각 클라이언트에게 생성한 플레이어 구조체 전달
            n = write(clie_sock[num_clients], &newPlayer, sizeof(newPlayer));
            if (n<0) {
                perror("write");
                exit(1);
            }

            num_clients++; idx++;
        }

        // 클라이언트의 행동 처리
        for (int i = 0; i < num_clients; i++) {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            int n = read(clie_sock[i], buffer, sizeof(buffer));
            if (n < 0) {
                perror("read");
                exit(1);
            }

            // 클라이언트의 액션에 따라 플레이어 업데이트
            if (strcmp(buffer, "hit") == 0) {
                // 플레이어가 hit한 경우 처리
                // 예시: players[i].score += cardValue(); // 카드 값에 따라 플레이어 점수 업데이트
            } else if (strcmp(buffer, "stand") == 0) {
                // 플레이어가 stand한 경우 처리
                // 예시: proceedGame(); // 게임 로직 진행
            }

            // 업데이트된 플레이어 정보를 클라이언트에 다시 전송
            // 예시: sendPlayerInfo(clie_sock[i], players[i]);
        }

        // 게임 루프 내에 다른 게임 로직과 조건을 추가할 수 있습니다.
    }

    // 연결된 클라이언트 소켓을 닫는 반복문
    for (int i = 0; i < num_clients; i++) {
        close(clie_sock[i]);
    }
    // 서버 소켓 닫음
    close(serv_sock);

    return 0;
}
