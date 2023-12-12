#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Card {
    int number;
    int shape;
};

void shuffle(struct Card card_all[]) {
    srand(time(NULL));
    for (int i = 0; i < 52; i++) {
        int index = rand() % 52;
        struct Card temp = card_all[index];
        card_all[index] = card_all[i];
        card_all[i] = temp;
    }
}

void filldeck(struct Card card_all[]) {
    int i = 0;
    // 4개의 모양과 13개의 숫자를 짝지어서 카드 만듦
    for (int j = 0; j < 4; j++) {
        for (int k = 1; k < 14; k++) {
            card_all[i].number = k;
            card_all[i].shape = j;
            i++;
        }
    }
}

int main() {
    struct Card card_a[52];

    filldeck(card_a); // 덱을 채우는 함수
    for (int i = 0; i < 52; i++) {
        printf("%d %d card list\n", card_a[i].number, card_a[i].shape);
    }
    printf("\n");
    shuffle(card_a);
    for (int i = 0; i < 52; i++) {
        printf("%d %d card list\n", card_a[i].number, card_a[i].shape);
    } // 카드를 섞는 함수
    return 0;
}
