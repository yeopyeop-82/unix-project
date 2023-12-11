#include "func.c"

int main() {
    int clie_sock, n, turn;
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
        n = read(clie_sock, &playerInfo.card_player, sizeof(playerInfo.card_player));
        if (n < 0) {
            perror("read");
            exit(1);
        }
        //자신이 뽑은 카드 확인
        printcard(playerInfo);
        turn = 4;
        
        //서버측에서 비동기적으로 1번부터 카드 배분하고, 한바퀴 돌거나 한쪽이 입력받을 때마다
        //turn이 몇번째인지 전달해줘야함
        //딜러측에서 한장 클라이언트 각 한장씩 뽑으면 turn은 4고, 
        //여기서 클라이언트1이 HIT을 두번 하면 클라이언트2의 turn은 7임
        n = read(clie_sock, &turn, sizeof(turn));
        if (n < 0) {
            perror("read");
            exit(1);
        }
        //stay or hit 결정
        stayorhit(playerInfo, turn);
        //진행한 턴 수 서버로 전달
        n = write(clie_sock, &turn, sizeof(turn));

    
        printf("결과: %s\n", buffer);
    break;
    }
    // 소켓 닫기
    close(clie_sock);

    return 0;
}
