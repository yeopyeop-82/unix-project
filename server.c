#include "func.c"

int main() {
    int serv_sock, clie_sock[MAX_CLI];
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    int n, idx = 1, num_clients = 0, clieturn=0;
    // 각 클라이언트에게 할당해줄 플레이어 구조체 선언
    struct Player players[MAX_CLI];
    // 딜러 구조체 선언
    struct Player servDealer;
    // 카드 덱 생성
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
        while (num_clients < MAX_CLI) {
            clie_sock[num_clients] = accept(serv_sock, (struct sockaddr *)&client_addr, &client_len);
            // 플레이어 데이터 초기화
            struct Player newPlayer;
            reset(&newPlayer); // reset() 함수 호출로 플레이어 데이터 초기화
            players[num_clients] = newPlayer; // 초기화된 데이터를 서버의 플레이어 배열에 할당
            newPlayer.idx = idx;// 플레이어 번호 부여

            //각 클라이언트에게 생성한 플레이어 구조체 전달
            n = write(clie_sock[num_clients], &newPlayer, sizeof(newPlayer));
            if (n<0) {
                perror("write");
                exit(1);
            }
            printf("Player%d 접속.\n", idx);
            num_clients++; idx++;

            // 3명 접속하면, 게임 시작 알림 보내기
            if (num_clients == MAX_CLI) {
                char *start_msg = "블랙잭 게임을 시작합니다!";
                for (int i = 0; i < num_clients; i++) {
                    n = write(clie_sock[i], start_msg, strlen(start_msg));
                    if (n < 0) {
                        perror("write");
                        exit(1);
                    }
                }
            }
        }
    
        //플레이어 초기화 후 덱 생성
        filldeck(card_all); // 덱을 채우는 함수
        shuffle(card_all); // 카드를 섞는 함수
        int turn = 0; // 생성한 덱에서 뽑는 순서

        // 게임 진행
        while(1) {
            struct Card c = card_all[turn];
            for (int i = 0; i < MAX_CLI; i++) {
                // client_sockets[i]에 해당하는 클라이언트로 값을 보냄
                n = write(clie_sock[i], &c, sizeof(c));
                if (n < 0) {
                    perror("write");
                    exit(1);
                }
                turn++; // 다음 카드
                struct Card c = card_all[turn];
            }
            //카드 배분이 끝난 후 클라이언트1부터 hitorstay진행하고 턴 반환
            for(int i = 0; i < MAX_CLI; i++) {
                //클라이언트가 턴을 받아 읽고 stayorhit() 진행후 턴을 다시 write()하면 서버에서 받아옴
                //이 부분은 비동기 처리 필요함
                while (1) {
                    //현재 턴을 첫번째 클라이언트에게 전송해주기
                    n = write(clie_sock[i], &turn, sizeof(turn));
                    if (n<0) {
                        perror("write");
                        exit(1);
                    }

                    //클라이언트 측에서 stayorhit() 진행 후 턴, player.score 반환
                    // 클라이언트의 액션에 따라 플레이어 업데이트
                    if (strcmp(buffer, "hit") == 0) {
                    // 플레이어가 hit한 경우 처리
                    // 예시: players[i].score += cardValue(); // 카드 값에 따라 플레이어 점수 업데이트
                    } else if (strcmp(buffer, "stand") == 0) {
                    // 플레이어가 stand한 경우 처리
                    // 예시: proceedGame(); // 게임 로직 진행
                    }
                    //클라이언트가 사용한 턴 수 읽어오기
                    n = read(clie_sock[i], &clieturn, sizeof(clieturn));
                    if (n < 0) {
                        perror("read");
                        exit(1);
                    }
                //해당 클라이언트가 진행한 턴 수 더해주기
                turn += clieturn;
                //해당 클라이언트 턴 종료, 다음 클라이언트 진행
                break;
                }
            }
            //클라이언트 플레이가 끝난 후 딜러 측 플레이 시작
            printcard(servDealer);

            // 업데이트된 플레이어 정보를 클라이언트에 다시 전송
            // 예시: sendPlayerInfo(clie_sock[i], players[i]);
            }
        }    

    // 연결된 클라이언트 소켓을 닫는 반복문
    for (int i = 0; i < num_clients; i++) {
        close(clie_sock[i]);
    }
    // 서버 소켓 닫음
    close(serv_sock);

    return 0;
}
