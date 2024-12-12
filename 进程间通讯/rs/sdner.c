#include <proto.h>

int main(){
    key_t key;
    int msgid;
    key = ftok(KEYPATH, KEYPROJ);
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