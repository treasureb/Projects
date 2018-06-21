#include"mp3.h"

void list_insert(char* music_name){
    list_node* p =(list_node*)malloc(sizeof(list_node));
    memset(p,0x00,sizeof(list_node));

    p->name = (char*)malloc(strlen(music_name)+1);
    strcpy(p->name,music_name);
    p->next = NULL;
    p->prev = NULL;

    if(g_head == NULL){
        p->next = p;
        p->prev = p;
        g_head = p;
        g_cur = g_head;
    }else{
        //画图理解指针指向
        p->prev = g_head->prev;
        p->next = g_head;
        g_head->prev->next = p;
        g_head->prev = p;
    }
}   //end of list_insert

void list_show( void ){
    list_node* cur = g_head;
    if(cur == NULL){
        return;
    }

    do{
        printf("%s",cur->name);
        if(g_cur == cur){
            printf("    <-: 当前播放歌曲");
        }
        printf("\n");
    }while( cur != g_head );
}

int check_music(char* music_name){
    char* str = strrchr(music_name,'.');
    if(str == NULL)
        return 0;
    else{
        str++;
        const char* mp3 = "mp3";
        if(strncasecmp(str,mp3,3))
            return 1;
        else
            return 0;
    }
}

void music_name( void ){
    DIR* pdir = opendir("./music");
    if(pdir == NULL){
        printf("当前没有歌曲,请尝试下载\n");
        exit(1);
    }
    struct dirent* pd = NULL;
    char buf[1024] = {0};
    while(( pd = readdir(pdir)) != NULL){
        if(pd->d_name[0] == '.')
            continue;
        if(!check_music(pd->d_name)){
            sprintf(buf,"./music/%s",pd->d_name);
            list_insert(buf);
            memset(buf,0x00,sizeof(buf));
        }
    }
    closedir(pdir);
}

void play_pause( void ){
    if(g_first == 1){//播放
        char buf[1024] = {0};
        //这里如果不放到后台执行,则在system函数执行结束前,是阻塞式的
        sprintf(buf,"madplay -o wav:- %s 2>/dev/null | aplay > /dev/null 2>&1 &",g_cur->name);
        system(buf);
        g_first = 0;
        g_status = PALY;
    }else{//暂停
        if(g_status == PALY){
            system("killadd -SIGSTOP aplay");
            g_status = PAUSE;
        }else if(g_status == PAUSE){
            system("killall -SIGCONT aplay");
            g_status = PALY;
        }
    } 
}

void stop(){
    system("killall -SIGKILL aplay");
    g_status = STOP;
    g_first = 1;
}

void next(){
    g_cur = g_cur->next;
    stop();
    play_pause();
}

void prev(){
    g_cur = g_cur->prev;
    stop();
    play_pause();
}

int menu( void ){
    printf("\033[5m \033[41m***************迷你播放器***************\n");
    list_show();
    printf("------------------------------\n");
    printf("\t1. 播放/暂停\n");
    printf("\t2. 下一首\n");
    printf("\t3. 上一首\n");
    printf("\t4. 停止\n");
    printf("\t0. 退出\n");
    printf(">");
    printf("\033[0m");
    int choose;
    scanf("%d",&choose);

    return choose;
}

void start( void ){
    int choose = 0;
    music_read();
    do{
        choose = menu();
        switch (choose){
        case 1:
            play_pause();
            break;
        case 2:
            next();
            break;
        case 3:
            prev();
            break;
        case 4:
            stop();
            break;
        case 0:
            system("killall -SIGKILL aplay");
            break;
        }
    }while(choose != 0);
}

int main( void ){
    music_read();
    start();
    return 0;
}
