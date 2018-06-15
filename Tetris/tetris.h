#ifndef __TETRIS_H__
#define __TETRIS_H__

#include<unistd.h> /* usleep */
#include<stdio.h>
#include"./keyboard/keyboard.h"

#define true  1
#define false 0
#define bool int 

#define FC 5
#define BC 0
#define W  10
#define H  20

/* 游戏的区域限定 */
int background[H][W];

struct data{
    int x;
    int y;
};

/* 每一个具体的图案*/  
struct shape{
    int s[5][5];
};

/* 绘制每一个小方块 */
 void draw_element(int x,int y,int colour);

/* 设置背景 */
 void set_back(struct data* t,struct shape p);

/* 绘制背景 */
 void draw_bk(void);

/* 绘制一种图形 */
 void draw_shape(int x,int,struct shape p,int c);

/* 判断位置是否可移动 */
 bool can_move(int x,int y,struct shape p);

/* 控制自动下落 */
 void tetris_timer(struct data* p);

/* 键盘控制图形的移动 */
 void tetris(struct data* p);

/* 当一行都满了,然后进行消行 */
 void clear_line();
#endif /* __TETRIS_H__ */
