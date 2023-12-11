#include "func.c"

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
    // 서버 주소 설정
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

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
    // 서버로부터 받은 본인의 플레이어 정보 출력
    printInfo(playerInfo);

    // 서버로부터 게임 시작 메시지 수신
    memset(buffer, 0, sizeof(buffer));
    n = read(clie_sock, buffer, sizeof(buffer));
    if (n < 0) {
        error("ERROR reading from socket");
    }
    printf("%s\n", buffer);

    while (1) {
        //서버로부터 카드 받기
        int n = read(clie_sock, &playerInfo.card_player, sizeof(playerInfo.card_player));
        if (n < 0) {
            perror("read");
            exit(1);
        }
        printcard(playerInfo);
        // 사용자의 선택 입력
        
        fgets(buffer, sizeof(buffer), stdin);
        int choice = atoi(buffer);

        // 클라이언트가 0을 입력하여 게임 종료 요청 시
        if (choice == 0) {
            write(clie_sock, buffer, strlen(buffer)); // 서버에 0 전송
            printf("게임을 종료합니다.\n");
            break;
        } else if (choice < 1 || choice > 3) {
            printf("잘못된 입력입니다. 가위(1), 바위(2), 보(3) 중에서 선택하세요.\n");
            continue; // 잘못된 입력일 경우 반복문 계속
        }

        // 사용자의 선택을 서버로 전송
        sprintf(buffer, "%d", choice);
        write(clie_sock, buffer, strlen(buffer));

        // 서버로부터 결과 수신
        memset(buffer, 0, sizeof(buffer));
        n = read(clie_sock, buffer, sizeof(buffer));
        if (n < 0) {
            error("ERROR reading from socket");
        }

        printf("결과: %s\n", buffer);
    break;
    }
    // 소켓 닫기
    close(clie_sock);

    return 0;
}
