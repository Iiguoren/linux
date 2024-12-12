#include <stdio.h>
#include <stdlib.h>
#include <proto.h>
int main(){
    key_t key;
    int msgid;
    key = ftok(KEYPATH, KEYPROJ);
    if(key<0){
        perror("ftok()");
        exit(1);
    }
    msgid = msgget(key, IPC_CREAT|0600);
    if(msgid<0){
        perror("msggt()");
        exit(1);
    }
    while(1){
    if(msgrcv(msgid, &rbuf, sizeof(rbuf)-sizeof(long), 0, 0)<0)
        {
            perror("msgrcv()");
            exit(1);
        }
        printf("NAME=%s\nMATH=%d\nChinese=%d\n",rbuf.name, rbuf.math,rbuf.chinese);
    }
    msgctl(msgid, IPC_RMID, NULL);
    exit(0);
}

