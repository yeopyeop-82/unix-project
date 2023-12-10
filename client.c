#include "func.c"

#define PORT 12345

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main() {
    int clie_sock, n;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char buffer[256];

    // 클라이언트 소켓 생성
    clie_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (clie_sock < 0) {
        error("ERROR opening socket");
    }

    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    //서버로부터 게임 시작 메시지 수신
    memset((char *)&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    // 서버에 연결 요청
    if (connect(clie_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
    }

    printf("서버에 연결되었습니다.\n상대 플레이어를 기다리는중...\n");

    // 서버로부터 플레이어 정보 수신
    struct Player playerInfo;
    n = read(clie_sock, &playerInfo, sizeof(playerInfo));
    if (n < 0) {
        perror("socket");
    }
    // 서버로부터 받은 플레이어 정보 출력
    printf("플레이어 정보: Player%d\n", playerInfo.idx);
    printf("점수: %d\n", playerInfo.score);
    printf("보유금: %d\n", playerInfo.cash);

    // 소켓 닫기
    close(clie_sock);

    return 0;
}
