#include <stdio.h>
#include <stdlib.h>

int main()
{
    int round,number,guess;
    time_t t;
    srand((unsigned) time(&t));
    number = rand() % 100;
    printf("This is guessing number game. In 7 rounds, player has to the number from 1 to 100\n");
    for(round = 1 ; round <= 7 ; round++){
        printf("Round %d, enter your number : ",round);
        scanf("%d",&guess);
        if(guess == number){
            printf("\nCongratulation! You win this game.\n");
            round = 9;
        }else if(guess < number){
            printf("Incorrect, your guessing is too low.\n");
        }else if(guess > number){
            printf("Incorrect, your guessing is too high.\n");
        }
    }
    if(round == 8){
        printf("\nYou lose. The correct number is %d.\n",number);
    }
    return 0;
}
