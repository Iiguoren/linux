#include <proto.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int main(){
    key_t key;
    int msgid;
    key = ftok(KEYPATH, KEYPROJ);
    msgid = msgget(key, IPC_CREAT|0600);
    if(key<0){
        perror("ftok()");
        exit(1);
    }

    sbuf.mtype = 1;
    strcpy(sbuf.name, "Linux");
    sbuf.math = rand()%100;

    if(msgsnd(msgid, &sbuf, sizeof(sbuf)-sizeof(long), 0)<0){
        {
            perror("msgsnd()");
            exit(1);
        }
    }
    //msgctl(msgid, IPC_RMID, NULL);
    puts("ok\n");
    exit(0);
}