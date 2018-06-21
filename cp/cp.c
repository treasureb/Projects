#include"common.h"

void cp_file(const char* src,const char* dst,mode_t mode){
    
    printf("目标%s\n",dst);
    int fd_src = open(src,O_RDONLY);
    if(fd_src == -1)
        ERROR("open src");
    int fd_dst = open(dst,O_WRONLY | O_CREAT,mode);
    if(fd_dst == -1)
        ERROR("open dst");

    char buf[1024];
    memset(buf,0x00,sizeof(buf));

    int read_size;
    while((read_size = read(fd_src,buf,1024)) > 0){
        write(fd_dst,buf,read_size);
        memset(buf,0x00,sizeof(buf));
    }

    close(fd_src);
    close(fd_dst);
}

void cp_dir(const char* src,const char* dst){
    DIR* pdir = opendir(src);
    if(pdir == NULL)
        ERROR("opendir");

    struct dirent* pd = NULL;
    while((pd = readdir(pdir)) != NULL){
        //如果是.则无限递归
        if(strcmp(pd->d_name,".") == 0 || strcmp(pd->d_name,"..") == 0) 
            continue;
        
        char tmp_src[1024] = {0};
        sprintf(tmp_src,"%s/%s",src,pd->d_name);
        struct stat sbuf;
        if(stat(tmp_src,&sbuf) == -1){
            ERROR("stat");
        }
        char tmp_dst[1024] = {0};
        sprintf(tmp_dst,"%s/%s",dst,pd->d_name);

        if(S_ISREG(sbuf.st_mode)){//原目录下是文件
            cp_file(tmp_src,tmp_dst,sbuf.st_mode);
        }else if(S_ISDIR(sbuf.st_mode)){//原目录下还是一个目录
            mkdir(tmp_dst,sbuf.st_mode);
            cp_dir(tmp_src,tmp_dst);
        }

    }
}


int main(int argc,char* argv[]){
    if(argc != 3){
        fprintf(stderr,"Usage :%s src dst",argv[0]);
        exit(-1);
    }

    struct stat sbuf;
    if(stat(argv[1],&sbuf) == -1)
        ERROR("stat error");
    if(S_ISREG(sbuf.st_mode)){//原文件是普通文件
        struct stat dbuf;
        if(stat(argv[2],&dbuf) == -1){//目标不存在,直接拷贝
            cp_file(argv[1],argv[2],sbuf.st_mode);
        }else if(S_ISDIR(dbuf.st_mode)){//目标文件是一个目录
            //格式:dir/file
            char tmp[1024] = {0};
            //利用sprintf进行字符串的拼接
            sprintf(tmp,"%s/%s",argv[2],argv[1]);
            cp_file(argv[1],tmp,sbuf.st_mode);
        }else if(S_ISREG(dbuf.st_mode)){//目标是一个已存在的文件
            //是否要覆盖原文件
            printf("是否要覆盖目标文件\n");
            char c;
            scanf("%c",&c);
            if(c == 'y' || c == 'Y'){
                //将文件截取为0个字符，即清除目标文件
                truncate(argv[2],0);
                cp_file(argv[1],argv[2],sbuf.st_mode);
            }

        }
        
    }else if(S_ISDIR(sbuf.st_mode)){//原文件是目录
        struct stat dbuf;
        if(stat(argv[2],&dbuf) == -1){//目标目录不存在
            mkdir(argv[2],sbuf.st_mode);
            cp_dir(argv[1],argv[2]);
        }else if(S_ISDIR(dbuf.st_mode)){//目标文件是一个目录
            char tmp[1024] = {0};
            sprintf(tmp,"%s/%s",argv[2],argv[1]);
            mkdir(tmp,sbuf.st_mode);
            cp_dir(argv[1],tmp);
        }else{//目标是一个文件
            printf("目录不能拷贝到文件中!\n");
            exit(0);
        }

    }else{//其他文件,暂不考虑

    }
}

