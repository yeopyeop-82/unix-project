#include "func.c"

int main()
{
    int serv_sock, clie_sock;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    int dealerScore, n, idx = 1, clieturn = 0;
    // 각 클라이언트에게 할당해줄 플레이어 구조체 선언
    struct Player players;
    // 딜러 구조체 선언
    struct Player servDealer;
    // 카드 덱 생성
    struct Card card_all[52];

    char choice, buffer[256];

    // 덱 생성
    filldeck(card_all); // 덱을 채우는 함수
    shuffle(card_all);  // 카드를 섞는 함수
    int turn = 0;       // 생성한 덱에서 뽑는 순서

    // 서버 소켓 생성
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0)
    {
        perror("socket");
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 소켓과 주소 바인딩
    if (bind(serv_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    // 최대 클라이언트 수 접속까지 연결 요청 대기
    listen(serv_sock, MAX_CLI);
    printf("서버 가동 시작.\n클라이언트 접속 대기...\n");

    client_len = sizeof(client_addr);
    // 클라이언트 연결 수락 및 플레이어 데이터 초기화
    clie_sock = accept(serv_sock, (struct sockaddr *)&client_addr, &client_len);
    // 플레이어 데이터 초기화
    reset(&players); // reset() 함수 호출로 플레이어 데이터 초기화
    printf("%d %d : playerInfo\n", players.cash, players.score);

    // client_sockets[i]에 해당하는 클라이언트로 값을 보냄
    n = write(clie_sock, &players, sizeof(players));
    if (n < 0)
    {
        perror("write");
        exit(1);
    }
    // 카드 정보 송신
    n = write(clie_sock, &card_all[turn], sizeof(card_all[turn]));
    printf("%d %d :card info/ %d :turn\n", card_all[turn].number, card_all[turn].shape, turn);
    turn++; // 다음 카드

    // 클라이언트가 달라는 대로 계속 카드 주기
    while (1)
    { // 클라이언트의 선택 받아오기
        n = read(clie_sock, &choice, sizeof(choice));
        if (choice == 'h' || choice == 'H')
        {
            // 카드 정보 송신
            n = write(clie_sock, &card_all[turn], sizeof(card_all[turn]));
            turn++; // 다음 카드
        }
        else
        {
            n = read(clie_sock, &players.score, sizeof(players.score));
            break;
        }
    }
    // 딜러 차례
    printf("\n+딜러 차례입니다.+\n\n");
    while (players.score < 21)
    {
        if (players.score < AI)
        {
            turn++;
            players.card_player[turn] = card_all[next++];
        }
        else
        {
            break;
        }
    }
    if (players.score > 21)
    {
        players.score = 0;
    }

    // 연결된 클라이언트 소켓을 닫는 반복문
    close(clie_sock);
    // 서버 소켓 닫음
    close(serv_sock);

    return 0;
}
