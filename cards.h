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

#define PORT 12345
#define HIT 2
#define STAY 1
#define AI 17
#define SPADE "♠"
#define CLUB "♣"
#define DIA "◆"
#define HEART "♥"
#define MAX_CLI 3

enum shape { spade, club, dia, heart };
enum num { ace = 1, jack = 11, queen = 12, king = 13 };

struct Card {
    int number;
    int shape;
};

struct Player {
    int idx;
    int cash;
    int score;
    struct Card card_player[21];
};

#endif /* CARDS_H */