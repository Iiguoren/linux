#ifndef POOL_PIPE_H
#define POOL_PIPE_H
#define PIPE_READER 0x00000001UL 
#define PIPE_WRITER 0x00000002UL
typedef void mypipe_t;

mypipe_t *mypipe_init();

int mypipe_destory(mypipe_t *);

int mypipe_read(mypipe_t , void *buf, int size);

int mypipe_write(mypipe_t , const void *buf, int size);



#endif