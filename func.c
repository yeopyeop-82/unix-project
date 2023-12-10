#include "cards.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define HIT 1
#define STAY 0
#define MAX_CLI 3
#define AI 17
#define SPADE "♠"
#define CLUB "♣"
#define DIA "◆"
#define HEART "♥"

int next;
struct Player player[3];
struct Card card_all[52];

//플레이어 데이터 초기화하는 함수, 서버
void reset(struct Player players[MAX_CLI]) {
    
    for (int i = 0; i < MAX_CLI; ++i) {
        players[i].idx = 0;
        players[i].score = 0; // 플레이어의 점수 초기화
        players[i].cash = 500; // 플레이어의 현금 초기화
    }
}

// 카드 섞는 함수, 서버
void shuffle()
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
void filldeck()
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

//플레이어 리스트, 카드 순서 받아서 한명씩 카드 배분 함수, 서버
void deal_players(struct Player players[MAX_CLI], int turns) {
  //deal
    for(int i = 0; i<3; i++) {
      printf("\n<Player%d 보유 금액: %d>", players[i].idx, players[i].cash);
      players[i].card_player[turns] = deal(next++);
    }
}

//플레이어 리스트, bet size 리스트를 인자로 받음, 서버
int bet_players(struct Player players[MAX_CLI], int betsize[MAX_CLI]) {
    //딜러의 베팅 금액 더함
    int bet_total = 0;

    //각 플레이어의 베팅 금액을 더함
    for(int i = 0; i<3; i++) {
      bet_total += betsize[i];
    }
    return bet_total;
}

// 사용자가 뽑은 카드를 문자열이 아닌 카드 모양으로 출력, 점수 계산
// 클라이언트에서 사용해 출력할 생각, 서버로부터 turns를 받아 호출할 생각.
void printcard(int p, int turns)
{
  int ace_score=0;

  printf("┌───────┐\n");

  switch(player[p].card_player[turns].shape)
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

  switch(player[p].card_player[turns].number)
  {
  case ace:
    printf("|   A   |\n");
    ace_score = -1;
    break;
  case jack:
    printf("|   J   |\n");
    player[p].score += 10;
    break;
  case queen:
    printf("|   Q   |\n");
    player[p].score += 10;
    break;
  case king:
    printf("|   K   |\n");
    player[p].score += 10;
    break;
  default:
    printf("|  %2d   |\n", player[p].card_player[turns].number);
    player[p].score += player[p].card_player[turns].number;
    break;
  }

  switch(player[p].card_player[turns].shape)
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

// ace 계산 묻기
  if(ace_score == -1)
  {
    printf("ACE가 나왔네요, [1/11]점으로 계산합니다: ");
    scanf("%d", &ace_score);
    player[p].score += ace_score;
  }

  printf("score : %d\n", player[p].score);

  return;
}

// HIT STAY 결정, 플레이어 정보 인자로 받음, HIT은 1 리턴, STAY는 0 리턴, 클라이언트
int stayorhit(struct Player player, int turns)
{
  char answer;

  printf("\n##########TURN : PLAYER%d님##########\n\n", player.idx);
  while(1)
  {
    printf("HIT을 원하시면 h를, STAY를 원하시면 s를 입력하세요[h/s]: ");
    scanf(" %c", &answer);

    if (answer == 'h' || answer == 'H')
    {
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

  if(player.score>21)
  {
    printf("21점을 넘었습니다. LOSS\n");
    player.score = 0;
  }
  else if (player.score==21)
  {
    printf("21점입니다. WIN\n");
  }
}

// 딜러는 지능적으로 16까지만 안전하게 여기고 hit, 딜러 알고리즘, 서버
void dealer(int turns)
{
  printf("\n##########DEALER's TURN##########\n\n");
  while(player[0].score < 21)
  {
    if (player[0].score < AI)
    {
      turns++;
      player[0].card_player[turns] = deal(next++);
      printcard(0, turns);
    }
    else
    {
      break;
    }
  }
  if(player[0].score>21) player[0].score = 0;
}

// 게임 진행
void play(struct Player players[MAX_CLI])
{
  int turns = 0; int push = 0; int push_flag = 0; int bet_total = 0;

  while(1)
  {
    int winner, winnerscore=-1;
    int bet[3]; bet[0] = 10;
    push_flag = 0;

    if (push == 0) bet_total = 0;

    for(int i = 0; i<3; i++) {
      if(players[i].cash <= 0) {
        printf("Player%d 파산. 100 Coin 다시 충전.\n", players[i].idx);
      }
      players[i].cash = 100;
    }
    
    for(int i = 0; i<3; i++) {
      player[i].score = 0;
    }

    player[0].card_player[turns] = deal(next++);
    printcard(0, turns);

    //deal
    for(int i = 0; i<3; i++) {
      printf("\n<Player%d>\n", players[i].idx);
      printf("보유 코인: %d\n", players[i].cash);
      players[i].card_player[turns] = deal(next++);
      printcard(i, turns);
    }

// 동점 우승이 한쌍이라도 있으면, 다음 게임으로 배팅 금액을 몰빵
    for (int i=0; i<3; i++)
    {
      if (winnerscore < player[i].score)
      {
        winner = i;
        winnerscore = player[i].score;
      }
    }
    printf("prize: ");

    for (int i=0; i<3; i++)
    {
      if (winnerscore == player[i].score && winner != i)
      {
        if (push == 0) push = 1;
        push_flag = 1;
      }
    }

    push *= push_flag;

    if(push == 1)
    {
      player[1].cash -= bet[1];
      player[2].cash -= bet[2];
      printf("0\n");
    }
    else if(winner == 0)
    {
      player[1].cash -= bet[1];
      player[2].cash -= bet[2];
      push = 0;
      printf("%d\n", bet_total);
    }
    else if(winner == 1)
    {
      player[1].cash += bet_total;
      player[2].cash -= bet[2];
      push = 0;
      printf("%d\n", bet_total);
    }
    else if(winner == 2)
    {
      player[1].cash -= bet[1];
      player[2].cash += bet_total;
      push = 0;
      printf("%d\n", bet_total);
    }

    printf("\n결과>> P1 CASH : %d / P2 CASH : %d\n", player[1].cash, player[2].cash);

    char answer;
    printf("계속하시겠습니까? ");
    scanf(" %c", &answer);

    if (answer=='y'||answer=='Y')
    {
      continue;
    }
    else
    {
      break;
    }
  }

  printf("게임이 종료됩니다.)");
}

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

//시작
// void start() {
//   char choice;
//   printf("블랙잭 게임에 오신 것을 환영합니다!\n시작하시겠습니까? [y/n] ");
//   scanf("%c", &choice);

//   while(choice!='Y' && choice!='y' && choice!='N' && choice!='n')
//   {
//     printf("이상한 문자를 입력하셨습니다. 다시 입력해주세요! [y/n] ");
//     scanf("%c", &choice);
//   }

//   if(choice=='Y' || choice=='y')
//   {
//     //reset();//가진 돈 초기화
//   }
//   else if(choice=='N' || choice=='n')
//   {
//     printf("게임을 종료합니다.\n");
//   }
//   return;
// }

