#include "func.c"

int main()
{
    int serv_sock, clie_sock;
    // server 소켓, client 소켓
    socklen_t client_len;
    // 소켓 주소 구조체
    struct sockaddr_in server_addr, client_addr;
    // 서버와 클라이언트 소켓 주소 정보를 저장하는 구조체
    int dealerScore, n, idx = 1, roundCount = 1;
    // 클라이언트에게 할당해줄 플레이어 구조체 선언
    struct Player players;
    // 딜러 구조체 선언
    struct Player servDealer;
    // 카드 덱 생성
    struct Card card_all[52];

    char choice, buffer[256];

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
    printf("서버 가동 시작.\n클라이언트 접속 대기...\n\n");

    client_len = sizeof(client_addr);
    // 클라이언트 연결 수락 및 플레이어 데이터 초기화
    clie_sock = accept(serv_sock, (struct sockaddr *)&client_addr, &client_len);
    // 플레이어 데이터 초기화

    while (1) // whileA
    {
        // 게임 시작 지점 -- Round 가 다시 시작되면 여기로 !!
        while (1) // whileB
        {
            printf("< ROUND %d >\n", roundCount);
            players.score = 0;

            // 카드 덱 생성
            filldeck(card_all); // 덱을 채우는 함수
            shuffle(card_all);  // 덱을 섞는 함수
            int turn = 0;       // 플레이어 간 진행되는 플레이어 턴에 해당함, 생성한 덱의 인덱스로 활용

            reset(&servDealer); // reset() 함수 호출로 딜러 초기화

            // client_sockets[i]에 해당하는 클라이언트로 값을 보냄
            n = write(clie_sock, &players, sizeof(players));

            // 카드 정보 송신
            n = write(clie_sock, &card_all[turn], sizeof(card_all[turn]));
            // 서버측에서 갖고 있는 플레이어 점수 정보에 첫 카드의 점수 더하기
            // ACE카드는 뽑은 시점의 플레이어 점수가 10점 이하인 경우 11점으로 간주,
            // 10점 초과인 경우 1점으로 간주함
            printf("Sending card num: %d\n", card_all[turn].number);

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
            // ACE가 아닌 경우 카드의 점수는 number와 같음
            else
            {
                player->score += card_all[turn].number;
                turn++;
            }

            // 클라이언트가 요청하는대로 계속 카드 주기
            while (1) // whileC
            {
                // 클라이언트의 선택 받아오기
                n = read(clie_sock, &choice, sizeof(choice));

                // 클라이언트의 선택이 HIT이라면
                if (choice == 'h' || choice == 'H')
                {
                    // 카드 정보 송신
                    n = write(clie_sock, &card_all[turn], sizeof(card_all[turn]));
                    // ACE 점수 처리
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
                    // ACE가 아닌 카드의 경우
                    else
                    {
                        players.score += card_all[turn].number;
                        turn++;
                        // 클라이언트의 총 점수가 21점을 초과해버린 경우 자동 패배 처리
                        if (players.score > 21)
                        {

                            break; // whileC 탈출
                        }
                        else if (players.score == 21)
                        {

                            break; // whileC 탈출
                        }
                    }
                }
                // 클라이언트가 stay한 경우
                else if (choice == 's' || choice == 'S')
                {
                    // 클라이언트측에서 계산한 클라이언트 측의 총 점수를 받아옴
                    n = read(clie_sock, &players.score, sizeof(players.score));
                    // 딜러 차례
                    while (1) // while D
                    {
                        // 딜러는 플레이어를 이기는 조건을 달성할 때까지 드로우 해야함.
                        if (servDealer.score < players.score && servDealer.score < 21)
                        {
                            printf("+딜러 차례입니다.+ 플레이어의 점수: %d\n", players.score);
                            servDealer.card_player->number = card_all[turn].number;
                            servDealer.card_player->shape = card_all[turn].shape;
                            printcard(&servDealer);
                            // ACE 경우의 수 처리
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
                            // ACE 아닌 경우의 수 처리
                            else
                            {
                                servDealer.score += servDealer.card_player->number;
                            }
                            // 플레이어 점수가 21 초과한 경우 자동 패배 처리
                            if (players.score > 21)
                            {
                                break; // whileD 탈출
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
                            break; // whileD 탈출
                        }
                        else
                        {
                            break; // whileD 탈출
                        }
                    }
                    break; // whileC 탈출
                }
                else if (choice == 'y' || choice == 'Y')
                {
                    roundCount++;
                    break; // whileC 탈출
                }
                else if (choice == 'n' || choice == 'N')
                {
                    if ((players.score > servDealer.score && players.score < 21) || players.score == 21 || players.score == servDealer.score)
                    {
                        printf("플레이어 승리!\n");
                    }
                    else
                    {
                        printf("플레이어 패배!\n");
                    }
                    printf("게임 종료\n");
                    // 연결된 클라이언트 소켓을 닫는 반복문
                    close(clie_sock);
                    // 서버 소켓 닫음
                    close(serv_sock);
                    return 0;
                }
            } // whileC

        } // whileB
        if ((players.score > servDealer.score && players.score < 21) || players.score == 21 || players.score == servDealer.score)
        {
            printf("플레이어 승리!\n");
            break;
        }
        else
        {
            printf("플레이어 패배!\n");
            break;
        }
    } // whileA

    printf("게임 종료\n");
    // 연결된 클라이언트 소켓을 닫는 반복문
    close(clie_sock);
    // 서버 소켓 닫음
    close(serv_sock);
    return 0;
}
