#include<stdio.h>
#include<sys/time.h>
#include<sys/resource.h>

#define BLOCK_SIZE 24   /*由于存在虚拟的data字段,sizeof不能正确计算meta长度*/

typedef struct s_block *t_block;
struct s_block{
    size_t size;    //数据区大小 
    t_block next;   //指向下个块
    t_block prev;   //指向上个块
    int free;       //标志空闲块
    void *ptr;      //Magic pointer，指向data
    int padding;    //填充四字节,保证meta块为8的倍数
    char data[1];   //虚拟字段,表示数据块的第一个字节,长度不应计入meta
};

/* 内存分配算法 */
/* 1.First fit  */
/*从头开始，使用第一个数据区大小大于要求size的块*/
/* 2.Best fit   */
/*从头开始，遍历所有块，使用数据区大小大于size且差值最小的块*/

t_block find_block(t_block *last,size_t size);

/* 开辟新的块 */
t_block extend_heap(t_block last,size_t s);

/* 分裂一个块 */
void split_block(t_block b,size_t s);

/* 按8字节对齐 */
size_t align8(size_t s);

/* malloc主体实现 */
void *malloc(size_t size);

/* calloc主体实现 */
void *calloc(size_t number,size_t size);

/* realloc主体实现 */
void *realloc(void *p,size_t size);

/* 内存复制方法 */
void copy_block(t_block src,t_block dst);

/* 内存释放 */
void free(void *p);

/* 碎片内存合并 */
t_block fusion(t_block b);

/* 检验地址合法性 */
int valid_addr(void *p);
