#include<stdlib.h>
#include"musicCombined.h"
#include<sys/time.h>
#define KEYMAPPING_LEN 29
/*char keyMapping[3][2] = {
    {'z', 0},
    {'s', 1},
    {'x', 3},
    {'d', 4},
    {'c', 5},
};*/
char keyMapping[] = {
    'z', 's', 'x', 'd', 'c', 'v', 'g', 'b', 'h', 'n', 'j', 'm', 'q', '2', 'w', '3', 'e', 'r', '5', 't', '6', 'y', '7', 'u', 'i', '9', 'o', '0', 'p'
};
int main(void){
    //initscr();
    //raw();
    //noecho();
    initialize();
    char c;
    //testing tension
    //printf("tensionValue:%f\n", applyTension(0, 1, 200, 0.1, -90, 100));
    printf("value:%f\n", getEnvolopeValue(combined.testEnvolope, 4000));
    //printf("tensionValue:%f\n", applyTension(0, 4, 3, 3, 50, 1.5));
    //printf("tensionValue:%f\n", applyTension(1, 3, 3, 3, -100, 0.5));
    struct timeval stop, start;
    //gettimeofday(&start, NULL);
    while((c = getchar()) && c != '1' && c != EOF){
        if (c == '\n'){
            //gettimeofday(&stop, NULL);
            //printf("timeDifference:%lu\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec-start.tv_usec);
            continue;
        }
        else if (c == '0'){
            //print active frequencies
            printFrequencies();
        }
        else if (c == '3'){
            printUnisonValues(&(combined.oscillators[0]));
        }
        else if (c>=97){
            for (int i = 0; i < KEYMAPPING_LEN; i++){
                if (c == keyMapping[i]){
                    addnote(i);
                    break;
                }
            }
        }
    }
    //endwin();
    terminate();
}