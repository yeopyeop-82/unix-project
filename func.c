#include "cards.h"

int next;
struct Player player[3];
struct Card card_all[52];

// 플레이어 데이터 초기화하는 함수, 서버
void reset(struct Player *players)
{
  players->score = 0;  // 플레이어의 점수 초기화
  players->cash = 500; // 플레이어의 현금 초기화
  players->bet = 0;
}
void betting(struct Player *player)
{
  int b;

  while (1)
  {
    printf("얼마를 베팅하시겠습니까? 현재 보유금액:%d\n", player->cash);

    scanf("%d", &b);

    while (getchar() != '\n')
      ; // 버퍼 비우기

    if (b > 0 && b <= player->cash)
    {
      if (b == player->cash)
      {
        player->bet += b;
        player->cash -= b;
        printf("All In...!\n\n");
        break; // 올바른 값 입력 시 루프 탈출
      }
      else
      {
        player->bet += b;
        player->cash -= b;
        printf("%d 베팅 완료!\n\n", b);
        break; // 올바른 값 입력 시 루프 탈출
      }
    }
    else
    {
      printf("올바른 값을 입력하세요 (0 < %d인 정수)\n", player->cash);
    }
  }
}

// 카드 섞는 함수, 서버
void shuffle(struct Card card_all[])
{
  srand(time(NULL));
  for (int i = 0; i < 52; i++)
  {
    int index = rand() % 52;
    struct Card temp = card_all[index];
    card_all[index] = card_all[i];
    card_all[i] = temp;
  }
}

// 카드 채우는 함수, 서버
void filldeck(struct Card card_all[])
{
  int i = 0;
  // 4개의 모양과 13개의 숫자를 짝지어서 카드 만듦
  for (int j = 0; j < 4; j++)
  {
    for (int k = 1; k < 14; k++)
    {
      card_all[i].number = k;
      card_all[i].shape = j;
      i++;
    }
  }
}

// 섞은 후 앞에서 부터 한장 나눠주는 함수
struct Card deal(int next)
{
  return card_all[next];
}

// 플레이어 리스트, bet size 리스트를 인자로 받음, 서버
int bet_players(struct Player players, int betsize)
{
  // 딜러의 베팅 금액 더함
  int bet_total = 0;

  // 각 플레이어의 베팅 금액을 더함
  for (int i = 0; i < 3; i++)
  {
    bet_total += betsize;
  }
  return bet_total;
}

// 사용자가 뽑은 카드를 문자열이 아닌 카드 모양으로 출력, 점수 계산
// 플레이어 구조체만 넘겨주면 카드 정보를 읽어 카드 모양을 출력해줌.
void printcard(struct Player *player)
{
  int ace_score = 0;

  printf("┌───────┐\n");

  switch (player->card_player->shape)
  {
  case spade:
    printf("|%s      |\n", SPADE);
    break;
  case club:
    printf("|%s      |\n", CLUB);
    break;
  case dia:
    printf("|%s      |\n", DIA);
    break;
  case heart:
    printf("|%s      |\n", HEART);
    break;
  }

  switch (player->card_player->number)
  {
  case ace:
    printf("|   A   |\n");
    break;
  case jack:
    printf("|   J   |\n");
    break;
  case queen:
    printf("|   Q   |\n");
    break;
  case king:
    printf("|   K   |\n");
    break;
  default:
    printf("|  %2d   |\n", player->card_player->number);
    break;
  }

  switch (player->card_player->shape)
  {
  case spade:
    printf("|      %s|\n", SPADE);
    break;
  case club:
    printf("|      %s|\n", CLUB);
    break;
  case dia:
    printf("|      %s|\n", DIA);
    break;
  case heart:
    printf("|      %s|\n", HEART);
    break;
  }

  printf("└───────┘\n");

  return;
}

// HIT STAY 결정, 플레이어 정보, 카드 턴 인자로 받음, 클라이언트
int stayorhit(struct Player player, int turn)
{
  char answer;

  printf("\n+         PLAYER 님의 차례!         +\n\n");
  while (1)
  {
    printf("HIT을 원하시면 h를, STAY를 원하시면 s를 입력하세요[h/s]: ");
    scanf(" %c", &answer);

    if (answer == 'h' || answer == 'H')
    {
      turn += 1;
      return HIT;
    }
    else if (answer == 's' || answer == 'S')
    {
      return STAY;
    }
    else
    {
      printf("잘못 입력하셨습니다.\nHIT이면 'h', STAY면 's'를 입력해주세요.");
    }
  }

  if (player.score > 21)
  {
    printf("21점 초과.\n패배하셨습니다.\n");
    player.score = 0;
  }
  else if (player.score == 21)
  {
    printf("21점입니다.\n승리하셨습니다.\n");
  }
}

// 플레이어 구조체 받아서 정보 출력해주는 함수, 클라이언트
void printInfo(struct Player *player)
{
  printf("┌────────────┐\n플레이어 정보\n");
  printf("보유금: %d\n└────────────┘\n", player->cash);
}

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

// ===================================================

// 게임 진행
// void play(struct Player players)
// {
//   int turns = 0;
//   int push = 0;
//   int push_flag = 0;
//   int bet_total = 0;

//   while (1)
//   {
//     int winner, winnerscore = -1;
//     int bet[3];
//     bet[0] = 10;
//     push_flag = 0;

//     if (push == 0)
//       bet_total = 0;

//     for (int i = 0; i < 3; i++)
//     {
//       if (players.cash <= 0)
//       {
//         printf("Player 파산. 100 Coin 다시 충전.\n");
//       }
//       players.cash = 100;
//     }

//     for (int i = 0; i < 3; i++)
//     {
//       players.score = 0;
//     }

//     player[0].card_player[turns] = deal(next++);

//     // deal
//     for (int i = 0; i < 3; i++)
//     {
//       printf("보유 코인: %d\n", players.cash);
//       players.card_player[turns] = deal(next++);
//     }

//     // 동점 우승이 한쌍이라도 있으면, 다음 게임으로 배팅 금액을 몰빵
//     for (int i = 0; i < 3; i++)
//     {
//       if (winnerscore < player[i].score)
//       {
//         winner = i;
//         winnerscore = player[i].score;
//       }
//     }
//     printf("prize: ");

//     for (int i = 0; i < 3; i++)
//     {
//       if (winnerscore == player[i].score && winner != i)
//       {
//         if (push == 0)
//           push = 1;
//         push_flag = 1;
//       }
//     }

//     push *= push_flag;

//     if (push == 1)
//     {
//       player[1].cash -= bet[1];
//       player[2].cash -= bet[2];
//       printf("0\n");
//     }
//     else if (winner == 0)
//     {
//       player[1].cash -= bet[1];
//       player[2].cash -= bet[2];
//       push = 0;
//       printf("%d\n", bet_total);
//     }
//     else if (winner == 1)
//     {
//       player[1].cash += bet_total;
//       player[2].cash -= bet[2];
//       push = 0;
//       printf("%d\n", bet_total);
//     }
//     else if (winner == 2)
//     {
//       player[1].cash -= bet[1];
//       player[2].cash += bet_total;
//       push = 0;
//       printf("%d\n", bet_total);
//     }

//     printf("\n결과>> P1 CASH : %d / P2 CASH : %d\n", player[1].cash, player[2].cash);

//     char answer;
//     printf("계속하시겠습니까? ");
//     scanf(" %c", &answer);

//     if (answer == 'y' || answer == 'Y')
//     {
//       continue;
//     }
//     else
//     {
//       break;
//     }
//   }

//   printf("게임이 종료됩니다.)");
// }

// 에러 메시지 출력 함수
// 배팅하는 함수, 베팅 금액이 너무 클 경우 다시 입력 받음
// int betting(struct Player player, int bet)
// {
//   printf("보유 금액 :%d \n얼마를 배팅하시겠습니까? ", player.cash);

//   if(bet > player.cash)
//   {
//     printf("돈이 부족합니다.\n다시 입력 : ");
//     scanf("%d", &bet);
//     return bet;
//   }
//   else return bet;
// }

// 시작
//  void start() {
//    char choice;
//    printf("Player가 다 모였습니다.\n게임을 시작하시겠습니까? [y/n] ");
//    scanf("%c", &choice);

//   while(choice!='Y' && choice!='y' && choice!='N' && choice!='n')
//   {
//     printf("이상한 문자를 입력하셨습니다. 다시 입력해주세요! [y/n] ");
//     scanf("%c", &choice);
//   }

//   if(choice=='Y' || choice=='y')
//   {
//     printf("게임을 시작합니다.\n");
//   }
//   else if(choice=='N' || choice=='n')
//   {
//     printf("게임을 종료합니다.\n");
//   }
//   return;
// }

// 딜러는 지능적으로 16까지만 안전하게 여기고 hit, 딜러 알고리즘, 서버
// void dealer(struct Player *player, int turn)
// {
//   printf("\n+딜러 차례입니다.+\n\n");
//   while (player->score < 21)
//   {
//     if (player->score < AI)
//     {
//       turn++;
//       player->card_player[turn] = card_all[next++];
//     }
//     else
//     {
//       break;
//     }
//   }
//   if (player->score > 21)
//     player->score = 0;
// }
