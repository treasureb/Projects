#include"tetris.h"

/* 游戏中一共会出现的其中基础图案  */
struct shape shape_arr[7] = {
    { 0,0,0,0,0, 0,0,1,0,0, 0,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,1,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,0,1,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,1,1,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,1,1,0,0, 0,0,1,1,0, 0,0,0,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,0,1,1,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0 }
};

struct shape trans_shape(struct shape p){
    for(int i = 0;i < 5;i++){
        for(int j = 0;j < 5;j++){
            p.s[i][j] = p.s[4-j][i];
        }
    }
}

void clear_line(){
    int total = 0;
    int i = 0;
    for(i = 0;i < H;++i){
        for(int j = 0;j < W;++j){
            if(background[i][j] != 0){
                total++;

            }
        }
    }
}

void draw_element(int x,int y,int c){
    x *= 2;    /* 每个方格是由两个两个字符构成，所以占两个位 */
    x++;       /* 坐标从1,1开始 */
    y++;
    printf("\033[%d;%dH",y,x);          /* 设置光标位置   */
    printf("\033[3%dm\033[4%dm",c,c);   /* 设置颜色       */
    printf("[]");                       /* 显示的方格     */
    printf("\033[?25l");                /* 隐藏光标       */
    printf("\033[0m");                  /* 消除设置的颜色 */      
    fflush(stdout);
}

 void draw_shape(int x,int y,struct shape p,int color){
    for(int i = 0;i < 5;i++){
        for(int j = 0;j < 5;j++){
            if(p.s[i][j] != 0)
                /* 相对位置加上偏移量 */
                draw_element(x+j,y+i,color);
        }
    }
}

 void set_back(struct data* t,struct shape p){
    for(int i = 0 ;i < 5;i++){
        for(int j = 0;j < 5;j++){
            if(p.s[i][j] != 0)
                /* 将已经落下的图形设置到背景里 */
                background[t->y+i][t->x+j] = 1;
        }
    }
}

 void draw_bk(void){
    for(int i = 0;i < H;i++){
        for(int j = 0;j < W;j++){
            if(background[i][j] == 0)
                draw_element(j,i,BC);
            else
                draw_element(j,i,FC);
        }
    }
}

 bool can_move(int x,int y,struct shape p){
    for(int i = 0;i < 5;i++){
        for(int j = 0;j < 5;j++){
            if(p.s[i][j] == 0)
                continue;
            if(y + i >=  H)
                return false;
            if(x + j < 0)
                return false;
            if(x + j >= W)
                return false;
            /* 下面已经有方块，产生叠加 */
            if(background[y+i][x+j] != 0)
                return false;
        }
    }
    return true;
}

 void tetris(struct data* p){
    int n = get_key();
    if(is_up(n)){
        trans_shape(p->x,p->y,shape_arr[0],FC);
    }
    else if(is_left(n)){
        draw_shape(p->x,p->y,shape_arr[0],BC);
        p->x--;
        draw_shape(p->x,p->y,shape_arr[0],FC);
    }else if(is_right(n)){
        draw_shape(p->x,p->y,shape_arr[0],BC);
        p->x++;
        draw_shape(p->x,p->y,shape_arr[0],FC);
    }
}

 void tetris_timer(struct data* p){
    draw_shape(p->x,p->y,shape_arr[0],BC);
    if(can_move(p->x,p->y+1,shape_arr[0])){
        p->y++;
        draw_shape(p->x,p->y,shape_arr[0],FC);
    }else{
        set_back(p,shape_arr[0]);
        draw_shape(p->x,p->y,shape_arr[0],FC);
        p->y = 1;
    }
}

