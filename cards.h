#ifndef CARDS_H
#define CARDS_H

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
#include <semaphore.h>

#define PORT 12345
#define HIT 2
#define STAY 1
#define AI 17
#define SPADE "♠"
#define CLUB "♣"
#define DIA "◆"
#define HEART "♥"
#define MAX_CLI 1

enum shape
{
    spade,
    club,
    dia,
    heart
};
enum num
{
    ace = 1,
    jack = 11,
    queen = 12,
    king = 13
};

struct Card
{
    int number;
    int shape;
};

struct Player
{
    int cash;
    int score;
    int bet;
    struct Card card_player[21];
};

#endif /* CARDS_H */