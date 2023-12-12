#include "func.c"

int main()
{
    int clie_sock, n, turn = 4;
    struct sockaddr_in server_addr;
    struct hostent *server;
    struct Player player;

    char choice, matchResult, buffer[256];

    // 클라이언트 소켓 생성
    clie_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (clie_sock < 0)
    {
        error("ERROR opening socket");
    }
    // 서버 주소 설정
    server = gethostbyname("localhost");
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    // 서버에 연결 요청
    if (connect(clie_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
    }

    printf("게임을 시작합니다!\n");

    while (1)
    {
        // 서버로부터 플레이어 정보 받기
        n = read(clie_sock, &player, sizeof(player));
        if (n < 0)
        {
            perror("read");
            exit(1);
        }
        // 카드 받기
        n = read(clie_sock, &player.card_player, sizeof(player.card_player));
        // 플레이어 정보 출력
        printInfo(&player);
        // 자신이 뽑은 카드 확인
        printcard(&player);

        printf("\n+         HIT or STAY         +\nh/s 입력:\n");
        scanf("%c", &choice);
        if (choice == 'h' || choice == 'H')
        {
            // 서버에 h 요청
            n = write(clie_sock, &choice, sizeof(choice));
            // 서버에서 카드 받아오기
            n = read(clie_sock, &player.card_player, sizeof(player.card_player));
            pritncard(&player);
        }
        else if (choice == 's' || choice == 'S')
        {
            // 서버에 점수 총합 전달
            n = write(clie_sock, &player.score, sizeof(player.score));
        }
        break;
        }
    close(clie_sock);
    return 0;
}
