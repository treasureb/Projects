#include"tetris.h"

struct data pos = {.x = 3,.y = 5};
int main(){
    init_keyboard();
    draw_bk();
    while(1){
        tetris_timer(&pos);
        tetris(&pos);
        usleep(100000);
    }
    recover_keyboard();
}
