#include "func.c"

int main() {
    int clie_sock, n, turn = 4;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char answer, matchResult, buffer[256];

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
        //stay를 선택한 경우 서버에 점수 전달하고 break
        if (answer == "s" || answer == "S") {
            n = write(clie_sock, &playerInfo.score, sizeof(playerInfo.score));
            if(n<0) {
                perror("write");
                exit(1);
            }
            break;
        }
        //서버로부터 카드 받기
        n = read(clie_sock, &playerInfo.card_player, sizeof(playerInfo.card_player));
        if (n < 0) {
            perror("read");
            exit(1);
        }
        //자신이 뽑은 카드 확인
        printcard(playerInfo);
        
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
        printf("\n+         PLAYER%d 님의 차례!         +\n\n", playerInfo.idx);

        while(1) {
        printf("HIT을 원하시면 h를, STAY를 원하시면 s를 입력하세요[h/s]: ");
        scanf(" %c", &answer);

        if (answer == 'h' || answer == 'H') {
            turn += 1;
            //서버에 h or H 요청 보내기, 아닌 경우 서버측에서는 점수를 받아 해당 플레이어의 승패 처리 후 게임 결과 반환해주면 됨
            n = write(clie_sock, &answer, sizeof(answer));
            if (n<0) {
                perror("write");
                exit(1);
            }
            //처음으로 돌아가 카드 받기
            break;
        }
        else if (answer == 's' || answer == 'S') {
            break;
        }
        else {
            printf("잘못 입력하셨습니다.\nHIT이면 'h', STAY면 's'를 입력해주세요.");
        }
        }
        //무조건 패배한 경우
        if(playerInfo.score>21) {
            printf("21점 초과.\n패배하셨습니다.\n");
        }
        //무조건 승리한 경우
        else if (playerInfo.score==21) {
            printf("21점입니다.\n승리하셨습니다.\n");
        }           
        //진행한 턴 수 서버로 전달
        n = write(clie_sock, &turn, sizeof(turn));
        if (n < 0) {
            prerror("write");
            exit(1);
        }
        printf("결과: %s\n", buffer);
        break;
    }
    //서버로부터 게임 결과 받아 오기
    n = read(clie_sock, &matchResult, sizeof(matchResult));
    if(n<0) {
        perror("read");
        exit(1);
    }
    // 소켓 닫기
    close(clie_sock);

    return 0;
}
