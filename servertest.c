#include "func.c"

int main() {
    struct Card card_all[52];
    filldeck(card_all);
    printf("filldeck test\n");
    for(int i = 0; i < 10; i++) {
            printf("%d, %c : card Information\n", card_all[i].number, card_all[i].shape);
        }
    printf("\nshuffle test\n");
    shuffle(card_all);
    for(int i = 0; i < 10; i++) {
            printf("%d, %c : card Information\n", card_all[i].number, card_all[i].shape);
        }
    return 0;
}