#include "func.c"
int main()
{
    // 변수 및 구조체 정의
    int clie_sock, n = 4;
    // clie_sock : 클라이언트 소켓, n : read, write 시 error 작용
    // int turn;

    struct sockaddr_in server_addr;
    // 소켓의 서버 주소 구조체

    struct hostent *server;
    // 호스트 정보를 저장하는 구조체 포인터 변수 server 선언

    struct Player player, dealer;
    // 플레이어와 딜러의 정보를 저장하는 구조체 변수 player와 dealer 선언

    char choice, matchResult, buffer[256];

    // 클라이언트 소켓 생성
    clie_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (clie_sock < 0) // 클라이언트 소켓 에러 처리
    {
        perror("socket");
        exit(1);
    }
    // 서버 주소 설정
    server = gethostbyname("localhost");
    if (server == NULL) // 서버 주소 설정 에러 처리
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    // 서버 주소 구조체(server_addr)의 주소 체계를 IPv4(AF_INET)로 설정

    memcpy((char *)&server_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    // 호스트 정보 구조체(server)에서 주소 정보를 서버 주소 구조체(server_addr)로 복사

    server_addr.sin_port = htons(PORT);
    // 서버 주소 구조체(server_addr)의 포트 번호를 네트워크 바이트 순서로 설정

    // 서버에 연결 요청
    if (connect(clie_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    printf("게임을 시작합니다!\n");
    // 딜러 정보 초기화
    reset(&dealer);

    // 서버로부터 플레이어 정보 받기
    n = read(clie_sock, &player, sizeof(player));

    // 카드 받기
    n = read(clie_sock, &player.card_player, sizeof(player.card_player));
    // 받은 카드 점수 더하기
    if (player.card_player->number == ace)
    {
        if (player.score <= 10)
        {
            printf("ACE 11점으로 처리되었습니다!\n");
            player.score += 11;
        }
        else
        {
            printf("ACE 1점으로 처리되었습니다!\n");
            player.score += player.card_player->number;
        }
    }
    else
    {
        player.score += player.card_player->number;
    }

    // 플레이어 정보 출력
    printInfo(&player);

    // 자신이 뽑은 카드 확인
    printcard(&player);
    printf("Player Score: %d\n", player.score);

    // 베팅 금액 결정
    betting(&player);

    while (1)
    {
        // 플레이어에게 HIT 또는 STAY 선택 요청
        printf("[HIT or STAY]\nh/s 중 입력하세요:");
        fflush(stdin);      // 기존에 입력된 내용이 있다면 비우기
        choice = getchar(); // 개행 문자 이전의 문자만 받아옴

        if (choice == 'h' || choice == 'H')
        {
            // 서버에 h 요청
            n = write(clie_sock, &choice, sizeof(choice));
            // 서버에서 카드 받아오기
            n = read(clie_sock, &player.card_player, sizeof(player.card_player));
            // 받은 카드 점수 더하기
            if (player.card_player->number == ace)
            {
                if (player.score <= 10)
                {
                    printf("ACE 11점으로 처리되었습니다!\n");
                    player.score += 11;
                }
                else
                {
                    printf("ACE 1점으로 처리되었습니다!\n");
                    player.score += player.card_player->number;
                }
            }
            else
            {
                player.score += player.card_player->number;
            }

            printcard(&player);
            printf("Player Score: %d\n", player.score);

            if (player.score > 21)
            {
                printf("패배하셨습니다!\n");
                player.bet = 0;

                // 서버에 점수 총합 전달
                n = write(clie_sock, &player.score, sizeof(player.score));

                break;
            }
            else if (player.score == 21)
            {
                player.bet *= 3;
                printf("\n+축하합니다! 승리하셨습니다! %d 획득!+\n", player.bet);
                break;
            }
        }
        else if (choice == 's' || choice == 'S')
        {
            // STAY 선택이나 점수가 21을 초과한 경우 서버에 전달하고 루프 종료
            n = write(clie_sock, &choice, sizeof(choice));
            printf("\n\n+플레이어 턴 종료!+\n");
            // 서버에 플레이어 점수 전송
            n = write(clie_sock, &player.score, sizeof(player.score));
            // 중계 모드 시작
            while (1)
            {
                if (dealer.score < 21 && dealer.score <= player.score)
                {
                    // 딜러는 플레이어를 승리하는 조건 달성할 때까지 드로우 해야함
                    n = read(clie_sock, &choice, sizeof(choice));

                    // 딜러가 뽑은 카드 가져오기
                    n = read(clie_sock, &dealer.card_player, sizeof(dealer.card_player));

                    // 받은 카드 점수 더하기
                    if (dealer.card_player->number == ace)
                    {
                        if (dealer.score <= 10)
                        {
                            printf("ACE 11점으로 처리되었습니다!\n");
                            dealer.score += 11;
                        }
                        else
                        {
                            printf("ACE 1점으로 처리되었습니다!\n");
                            dealer.score += dealer.card_player->number;
                        }
                    }
                    else
                    {
                        dealer.score += dealer.card_player->number;
                    }
                    if (dealer.score == 0)
                    {
                        break;
                    }
                    sleep(1);
                    printf("\n<딜러 HIT>\n");
                    printcard(&dealer);
                    printf("Dealer Score: %d\n", dealer.score);
                }

                else if (dealer.score > 21)
                {
                    // 플레이어 승리 조건
                    player.bet *= 2;
                    printf("\n+축하합니다! 승리하셨습니다! %d 획득!+\n", player.bet);
                    break;
                }
                else
                {
                    // 플레이어 패배 조건
                    printf("\n+패배하셨습니다.+\n");
                    player.bet = 0;
                    break;
                }
            }
            break;
        }
    }
    // 베팅금액 정산
    player.cash += player.bet;
    player.bet = 0;
    printInfo(&player);
    // 소켓 닫기
    close(clie_sock);
    return 0;
}
