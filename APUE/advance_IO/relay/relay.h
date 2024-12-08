#ifndef RELAY_H__
#define RELAY_H__
#define BUFSIZE 1024
#define JOBMAX 1024
static struct job_st* jobs[JOBMAX];
enum {
    STATE_R = 1,
    STATE_W,
    STATE_Ex,
    STATE_T
};
enum {
    STATE_RUNNING = 1,
    STATE_CANCELD,
    STATE_OVER
};
struct fsm_st{
    int sfd;
    int dfd;
    int state;
    int pos;
    int len;
    char buf[BUFSIZE];
    char *errstr;
    long count;
    int name;
};

struct job_st
{
    int fd1, old_fd1;
    int fd2, old_fd2; //  接受传递的fd并对文件权限做处理
    struct fsm_st fsm12, fsm21;
    int state;
};

int add_job(int fd1, int fd2);
int cancel_job(int id);
int wait_job(int id);
int stat_job(int id);
#endif