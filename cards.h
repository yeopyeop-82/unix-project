#ifndef CARDS_H
#define CARDS_H

#define MAX_CLI 3

enum shape { spade, club, dia, heart };
enum num { ace = 1, jack = 11, queen = 12, king = 13 };

struct Card {
    int number;
    char shape;
};

struct Player {
    int idx;
    int cash;
    int score;
    struct Card card_player[21];
};

void shuffle();
void filldeck();
struct Card deal(int next);
void printcard(int p, int turns);
// int betting(struct Player player);
int stayorhit(struct Player player, int turns);
void dealer(int turns);
void play(struct Player players[MAX_CLI]);
void reset(struct Player players[MAX_CLI]);
//void start();

#endif /* CARDS_H */