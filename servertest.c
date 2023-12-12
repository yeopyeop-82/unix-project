#include "func.c"

int main() {
    struct Card card_all[52];

    filldeck(card_all); // 덱을 채우는 함수
    for(int i = 0; i<20; i++) {
    printf("%d %d card list\n", card_all->number, card_all->shape);
    }
    printf("\n");
    shuffle(card_all);
    for(int i = 0; i<20; i++) {
    printf("%d %d card list\n", card_all->number, card_all->shape);
    } // 카드를 섞는 함수
    return 0;
}