#include "func.c"

int main()
{
    int serv_sock, clie_sock;
    // server 소켓, client 소켓
    socklen_t client_len;
    // 소켓 주소 구조체
    struct sockaddr_in server_addr, client_addr;
    // 서버와 클라이언트 소켓 주소 정보를 저장하는 구조체
    int dealerScore, n, idx = 1;
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
    reset(&players);    // reset() 함수 호출로 플레이어 데이터 초기화
    reset(&servDealer); // reset() 함수 호출로 딜러 초기화

    // client_sockets[i]에 해당하는 클라이언트로 값을 보냄
    n = write(clie_sock, &players, sizeof(players));

    // 카드 정보 송신
    n = write(clie_sock, &card_all[turn], sizeof(card_all[turn]));
    // 서버측에서 갖고 있는 플레이어 점수 정보에 첫 카드의 점수 더하기
    if (card_all[turn].number == ace)
    {
        if (players.score <= 10)
        {
            printf("ACE 11점으로 처리되었습니다!\n");
            players.score += 11;
            turn++;
        }
        else
        {
            printf("ACE 1점으로 처리되었습니다!\n");
            players.score += card_all[turn].number;
            turn++; // 다음 카드
        }
    }
    else
    {
        player->score += card_all[turn].number;
        turn++;
    }

    // 클라이언트가 달라는 대로 계속 카드 주기
    while (1)
    { // 클라이언트의 선택 받아오기
        n = read(clie_sock, &choice, sizeof(choice));

        if (choice == 'h' || choice == 'H')
        {
            // 카드 정보 송신
            n = write(clie_sock, &card_all[turn], sizeof(card_all[turn]));
            if (card_all[turn].number == ace)
            {
                if (players.score <= 10)
                {
                    printf("ACE 11점으로 처리되었습니다!\n");
                    players.score += 11;
                    turn++;
                }
                else
                {
                    printf("ACE 1점으로 처리되었습니다!\n");
                    players.score += card_all[turn].number;
                    turn++; // 다음 카드
                }
            }
            else
            {
                players.score += card_all[turn].number;
                turn++;
                if (players.score > 21)
                {
                    printf("플레이어 패배!\n");
                    break;
                }
                else if (players.score == 21)
                {
                    printf("플레이어 승리!\n");
                    break;
                }
            }
        }
        else
        {
            // stay했거나 점수 21 초과한 경우
            n = read(clie_sock, &players.score, sizeof(players.score));
            // 딜러 차례
            while (1)
            {
                // 딜러는 플레이어를 이기는 조건을 달성할 때까지 드로우 해야함.
                if (servDealer.score < players.score && servDealer.score < 21)
                {
                    printf("+딜러 차례입니다.+ 플레이어의 점수: %d\n", players.score);
                    servDealer.card_player->number = card_all[turn].number;
                    servDealer.card_player->shape = card_all[turn].shape;
                    printcard(&servDealer);
                    if (servDealer.card_player->number == ace)
                    {
                        if (servDealer.score <= 10)
                        {
                            printf("ACE 11점으로 처리되었습니다!\n");
                            servDealer.score += 11;
                        }
                        else
                        {
                            printf("ACE 1점으로 처리되었습니다!\n");
                            servDealer.score += servDealer.card_player->number;
                        }
                    }
                    else
                    {
                        servDealer.score += servDealer.card_player->number;
                    }
                    if (players.score > 21)
                    {
                        printf("\n플레이어 패배!\n");
                        break;
                    }
                    printf("Dealer Score: %d\n", servDealer.score);
                    turn++;
                    // 클라이언트에 h 알림
                    n = write(clie_sock, &choice, sizeof(choice));

                    // 클라이언트에 딜러가 뽑은 카드 알림
                    n = write(clie_sock, &servDealer.card_player, sizeof(servDealer.card_player));
                }
                else if (servDealer.score <= 21 && servDealer.score > players.score)
                {
                    // 딜러의 승리 조건을 딜러의 점수를 계산한 후에 비교
                    printf("플레이어 패배!\n");
                    break;
                }
                else
                {
                    printf("플레이어 승리!\n");
                    break;
                }
            }
            break;
        }
    }

    printf("게임 종료\n");
    // 연결된 클라이언트 소켓을 닫는 반복문
    close(clie_sock);
    // 서버 소켓 닫음
    close(serv_sock);

    return 0;
}
