#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "medialib.h"
#include <string.h>
#include "../../include/proto.h"
#include "../../include/site_type.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include "mytbf.h"
#include "server_conf.h"
#include <syslog.h>
#define PATHSIZE 1024

#define PATHSIZE 1024
#define LINEBUFSIZE 1024
#define MP3_BITRATE 320 * 1024 // 采样率
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

int mlib_freechnlist(struct mlib_listentry_st *ptr) {
  free(ptr);
  return 0;
}
static struct channel_context_st *path2entry(const char* path){
    syslog(LOG_INFO,"current path:%s\n",path);
    char pathstr[PATHSIZE] = {'\0'};
    FILE *fp;
    int curr_id=MINCHNID;
    char linebuf[PATHSIZE];
    struct channel_context_st *me;
    strcat(pathstr, path);
    strcat(pathstr, "/desc.txt");
    // 检测：desc存在
    fp = fopen(pathstr,"r");
    if(fp == NULL){
        syslog(LOG_INFO,"%s is not a channel.", path);
        return NULL;
    }
    // 检测：desc内容存在
    if(fgets(linebuf, PATHSIZE, fp) == NULL){
        syslog(LOG_INFO, "%s is not a channel.", path);
        fclose(fp);
        return NULL;
    }
    syslog(LOG_DEBUG, "desc exit.");
    // 初始化
    me=malloc(sizeof(*me));
    if(me == NULL){
        syslog(LOG_ERR, "malloc():%s",strerror(errno));
        return NULL;  
    }
    // 每个频道初始化一个桶
    me->tbf = mytbf_init(MP3_BITRATE / 8, MP3_BITRATE / 8 * 5);
    if (me->tbf == NULL) {
        syslog(LOG_ERR, "mytbf_init():%s", strerror(errno));
        free(me);
        return NULL;
    }
    syslog(LOG_DEBUG, "tbf init finished.");
    me->desc = strdup(linebuf); //将描述从Linbuf写入
    strncpy(pathstr, path, PATHSIZE);
    syslog(LOG_DEBUG, "desc: %s",linebuf);
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
// 接收一个节目结构体数组，存储每个频道， 返回有用的结构体数量
int mlib_getchnlist(struct mlib_listentry_st **result, int *resnum){
    int i;
    int num;
    glob_t globres;
    struct mlib_listentry_st *ptr;
    struct channel_context_st *res;
    char path[PATHSIZE];
    
    // 初始化数组
    for(i = 1; i < MAXCHNID+1; i++)
    {
        channel[i].chnid = -1;
    }
    //将格式化的输出写入一个字符数组，写入media路径
    snprintf(path, PATHSIZE, "%s/*", server_conf.media_dir);

    //匹配路径下所有的文件
    if(glob(path, 0, NULL, &globres)<0)
    {
        syslog(LOG_ERR, "glob() error");
        return -1;
    }
    /*给指针分配，之所以动态分配，是因为大数组在堆上更合理；栈空间小*/ 
    // 先分配已有频道数量的内存,如果有不符合规范的频道后面减少内存分配
    ptr = malloc(sizeof(struct mlib_listentry_st) * globres.gl_pathc);
    if(ptr == NULL){
        syslog(LOG_ERR, "malloc() error");
        return -1;
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
    return -1;
    }
    *resnum = num;
    return 0;
}
static int open_next(chnid_t chnid){
    for(int i=0; i<channel[chnid].mp3glob.gl_pathc;i++){
        channel[chnid].pos++;
        if(channel[chnid].pos == channel[chnid].mp3glob.gl_pathc){
            channel[chnid].pos = 0;
            break;
            //syslog();
        }
        close(channel[chnid].fd);
        channel[chnid].fd = open(channel[chnid].mp3glob.gl_pathv[channel[chnid].pos], O_RDONLY);
        if(channel[chnid].fd<0){
            syslog(LOG_WARNING, "open(%s):%s",channel[chnid].mp3glob.gl_pathv[channel[chnid].pos],strerror(errno));
        }
        else{
            channel[chnid].offset = 0;
            return 0;
        }
    }
    syslog(LOG_ERR, "none of mp3 in channel %d is available.",chnid);
}
ssize_t mlib_readchn(chnid_t chnid, void *buf, size_t size){
    int len;
    int tbfsize;
    // 从令牌桶获得令牌
    tbfsize = mytbf_fetchtoken(channel[chnid].tbf, size);
    while(1){
        // 从媒体库中读取tbfsize个字节，存入buf,偏移量offset
        len = pread(channel[chnid].fd, buf, tbfsize, channel[chnid].offset);
        if(len < 0){
            syslog(LOG_WARNING,"media file pread():%s,%s",channel[chnid].mp3glob.gl_pathv[channel[chnid].pos] ,strerror(errno));
            open_next(chnid);
        }
        else if(len == 0){
            // 结束就下一首
            syslog(LOG_DEBUG,"media file %s is over",channel[chnid].mp3glob.gl_pathv[channel[chnid].pos]);
            open_next(chnid);          
            }
        else{
            // 保存offset
            channel[chnid].offset += len;
            break;
            }
        }
    if(tbfsize - len > 0)
        // 归还令牌
        mytbf_returntoken(channel[chnid].tbf, tbfsize-len);
    return len;
}
