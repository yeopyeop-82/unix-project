#ifndef CARDS_H
#define CARDS_H

enum shape { spade, club, dia, heart };
enum num { ace = 1, jack = 11, queen = 12, king = 13 };

struct Card {
    int number;
    char shape;
};

struct Player {
    int cash;
    int score;
    struct Card card_player[21];
};

void shuffle();
void filldeck();
struct Card deal(int next);
void printcard(int p, int turns);
int betting(struct Player player);
void stayorhit(int p, int turns);
void dealer(int turns);
void play();
void reset();
void start();

#endif /* CARDS_H */