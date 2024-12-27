#include <stdio.h>
#include <stdlib.h>
#include "medialib.h"
#include "../../include/proto.h"
#include <errno.h>

#define PATHSIZE 1024
struct channel_context_st{
    chnid_t chnid;
    char *desc;
    glob_t mp3glob; //目录项
    int pos;
    int fd;
    off_t offset;
    mytbf_t *tbf;
};
// 从媒体库获取节目单,存入listentry数组中，在server.c调用
struct channel_context_st channel[MAXCHNID+1];
static struct path2entry(const char* path){
    syslog(LOG_INFO,"current path:%s\n",path);
    char pathstr[PATHSIZE] = {'\0'};
    FILE *fp;
    int curr_id=MINCHNID;
    char linebuf[PATHSIZE];
    struct channel_context_st *me;
    strcat(pathstr, path);
    strcat(pathstr, "/desc.txt");
    // 检测：desc存在->desc内容存在
    fp = fopen(pathstr,"r");
    if(fp == NULL){
        syslog(LOG_INFO,"%s is not a channel.", path);
        return NULL
    }
    if(fgets(linebuf, PATHSIZE, fp) == NULL){
        syslog(LOG_INFO, "%s is not a channel.", path);
        fclose(fp);
        return NULL;
    }

    // 初始化
    me->malloc(sizeof(*me));
    if(me == NULL){
        syslog(LOG_ERR, "malloc():%s",strerr(errno));
        return NULL;  
    }
    // ?
    me->tbf = mytbf_init(MP3_BITRATE / 8, MP3_BITRATE / 8 * 5);
    if (me->tbf == NULL) {
        syslog(LOG_ERR, "mytbf_init():%s", strerror(errno));
        free(me);
        return NULL;
    }

    me->desc = strdup(linebuf); //将描述从Linbuf写入
    strncpy(pathstr, path, PATHSIZE);
    strncat(pathstr, "/*.mp3", PATHSIZE-1);
    // glob成功返回0
    if(glob(pathstr, 0, NULL, &me->mp3glob)!=0){
        curr_id++;
        syslog(LOG_ERR, "%s is not a channel dir(can not find mp3 files", path);
        free(me);
        return NULL;
    }
    me->pos = 0;
    me-> offset = 0;
    me->fd = open(me->mp3glob.gl_pathv[me->pos], O_RDONLY); // 打开第一个音乐文件
    if(me->fd <0 ){
        syslog(LOG_WARNING, "%s open failed.", me->mp3glob.gl_pathv[me->pos]);
        free(me);
        return NULL;
    }
    me->chnid = curr_id;
    curr_id++;
    return me;
} 
int mlib_getchnlist(struct mlib_listentry_st **result, int *resnum){
    int i;
    int num;
    glob_t globres;
    struct mlib_listentry_st *ptr;
    struct channel_context_st *res;
    char path[PATHSIZE];
    

    for(i = 1; i < MAXCHNID+1; i++)
    {
        channel[i].chnid = -1;
    }
    //将格式化的输出写入一个字符数组，写入media路径
    snprintf(path, PATHSIZE, "%s/*", server_conf.media_dir);

    //匹配路径下所有的文件
    if(glob(path, 0, NULL, &globres)<0)
    {
        return -1;
    }
    /*给指针分配，之所以动态分配，是因为大数组在堆上更合理；栈空间小*/ 
    // 先分配已有频道数量的内存,如果有不符合规范的频道后面减少内存分配
    ptr = malloc(sizeof(struct mlib_listentry_st) * globres.gl_pathc);
    if(ptr == NULL){
        syslog(LOG_ERR, "malloc() error");
    }
    // 将gl_pathv中的文件字符串写入result结构体数组
    for(i = 0; i<globres.gl_pathc; ++i){
        // globres.gl_pathv[i] -> "/var/media/ch1"
        res = path2entry(globres.gl_pathv[i]);
        if (res != NULL) {
        syslog(LOG_ERR, "path2entry() return : %d %s.", res->chnid, res->desc);
        // channel+chnid表示结构体数组的起始地址，memcpy用于将指针指向的地址复制到结构体数组中
        memcpy(channel + res->chnid, res, sizeof(*res));
        // 等价于*(ptr + num)
        ptr[num].chnid = res->chnid;
        ptr[num].desc = strdup(res->desc);
        num++;
        }
    }
    *result = realloc(ptr, sizeof(struct mlib_listentry_st) * num);
    if (*result == NULL) {
    syslog(LOG_ERR, "realloc() failed.");
    }
    *resnum = num;
    return 0;
}

