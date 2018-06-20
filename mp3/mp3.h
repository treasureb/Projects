#ifndef __MP3_H__
#define __MP3_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>

typedef enum{STOP,PALY,PAUSE}Status;    //当前状态

typedef struct list_node{               //使用list来保存列表歌曲
    char* name;
    struct list_node* prev;
    struct list_node* next;
}list_node;

list_node* g_head = NULL;                 //指向链表的头部
list_node* g_cur = NULL;                  //当前歌曲的节点
int g_first = 1;                          //用来标识是播放还是暂停
int g_status = STOP;                      //用来标识当前状态

void list_insert(char* music_name);           //往链表中插入一个节点
void list_show(void);                   //显示所有歌曲
void music_read(void);                  //读取音乐目录
int menu(void);                         //显示菜单
void play_pause();                      //播放或停止
void stop();                            //暂停播放器
void next();                            //下一曲
void prev();                            //上一曲
int check_music(char* music_name);      //检测是否为MP3格式的歌曲
void start();                           //游戏启动

#endif// end of __MP3_H__
