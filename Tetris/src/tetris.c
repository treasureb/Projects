#include"tetris.h"



int g_shape_cur_index = 0;
int g_shape_next_index = 0;

int score = 0;/* 分数 */
/* 游戏中一共会出现的其中基础图案  */
struct shape shape_arr[7] = {
    { 0,0,0,0,0, 0,0,1,0,0, 0,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,1,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,1,1,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,1,1,0,0, 0,0,1,1,0, 0,0,0,0,0, 0,0,0,0,0 },
    { 0,0,0,0,0, 0,0,1,1,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0 }
};

struct shape tip[1] = {
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1
};

struct shape trans_shape(struct shape p){
    struct shape tmp;
    for(int i = 0;i < 5;i++){
        for(int j = 0;j < 5;j++){
            tmp.s[i][j] = p.s[4-j][i];
        }
    }
    return tmp;
}

int rand_number(){
    return rand()%7;
}

void init_shape_arg(){
    
    /* 先将屏幕清除 */
    system("clear");

    /* 初始化时间种子 */
    srand((unsigned)(time(NULL)));
    
    /* 初始化随机数 */
    g_shape_cur_index = 0;
    g_shape_next_index = rand_number();
    

    /* 初始化键盘 */
    init_keyboard();

    /* 绘制游戏背景区域 */
    draw_bk();

}

void clear_line(){
    int i = 0;
    for(i = 0;i < H;i++){
        int total = 0;
        for(int j = 0;j < W;j++){
            if(background[i][j] != 0){
                total++;
            }
        }
#ifdef __DEGUG__
        printf("总块数：%d，已有块数：%d\n",W,total);
#endif /* __DEGUG__ */
        if(total == W){//一行是满的,统一将上一行直接复制到当前行
            for(int k = i;k > 0;k--){
                memcpy(background[k],background[k-1],sizeof(background[k]));
            }
            memset(background[g_shape_cur_index],0x00,sizeof(background[g_shape_cur_index]));
        }
    } 
}

void game_over(){
    for(int i = 0;i < W;i++){
        if(background[3][i] != 0){
            printf("\033[?25h");
            system("clear");
            exit(0);
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
     /* 绘制游戏区界面 */
    for(int i = 0;i < H;i++){
        for(int j = 0;j < W;j++){
            if(background[i][j] == 0)
                draw_element(j,i,BC);
            else
                draw_element(j,i,FC);
        }
    }

    /* 绘制提示区界面 */
    struct data tip_pos = {.x = 14,.y = 5};
    printf("\033[%d;%dH \033[31m 当前分数: %d\n",3,26,score);
    printf("\033[%d;%dH \033[31m 下一个图形: ",4,26);
    draw_shape(tip_pos.x,tip_pos.y,tip[0],BC);
    draw_shape(tip_pos.x,tip_pos.y,shape_arr[g_shape_next_index],FC);

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
        draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],BC);
        shape_arr[g_shape_cur_index] = trans_shape(shape_arr[g_shape_cur_index]);
        if(can_move(p->x,p->y,shape_arr[g_shape_cur_index]) == false){
            shape_arr[g_shape_cur_index] = trans_shape(shape_arr[g_shape_cur_index]);
            shape_arr[g_shape_cur_index] = trans_shape(shape_arr[g_shape_cur_index]);
            shape_arr[g_shape_cur_index] = trans_shape(shape_arr[g_shape_cur_index]);
        }
        draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
    }else if(is_left(n)){
        draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],BC);
        if(can_move(p->x-1,p->y,shape_arr[g_shape_cur_index])){
            p->x--;
            draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
        }else{
            draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
        }
    }else if(is_right(n)){
        draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],BC);
        if(can_move(p->x+1,p->y,shape_arr[g_shape_cur_index])){
            p->x++;
            draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
        }else{
            draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
        }
    }else if(is_down(n)){
        draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],BC);
        if(can_move(p->x,p->y+1,shape_arr[g_shape_cur_index])){
            p->y++;
            draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
        }else{
            draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
//            clear_line();
//            game_over();
//            draw_bk();
        }
    }
}

 void tetris_timer(struct data* p){
    draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],BC);
    if(can_move(p->x,p->y+1,shape_arr[g_shape_cur_index])){
        p->y++;
        draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
    }else{
        set_back(p,shape_arr[g_shape_cur_index]);
        draw_shape(p->x,p->y,shape_arr[g_shape_cur_index],FC);
        clear_line();
        game_over();
        p->y = 1;
        p->x = W/3;
        /* printf("产生随机数\n"); */
        g_shape_cur_index = g_shape_next_index;
        g_shape_next_index = rand_number();
        draw_bk();
    }
}

