#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mytbf.h"
#define SIZE 1024

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stdout, "Usage...\n");
        exit(1);
    }

    struct mytbf_st *tbf;
    tbf = mytbf_init(10, 100);
    if (tbf == NULL) {
        fprintf(stderr, "tbf init error\n");
        exit(1);
    }

    int sfd, dfd=0;
    do {
        sfd = open(argv[1], O_RDONLY);
        if (sfd < 0) {
            if (errno == EINTR)
                continue;
            perror("open()");
            exit(1);
        }
    } while (sfd < 0);

    // Open destination file
    dfd = 1;
    if (dfd < 0) {
        perror("open destination file");
        close(sfd);
        exit(1);
    }
    setbuf(stdout, NULL);
    char buf[1024];
    while (1) {
        int len, ret, pos = 0;
        int size;
        size = mytbf_fetchtoken(tbf, SIZE);
        if (size < 0) {
            fprintf(stderr, "mytbf_fetchtoken() error: %s\n", strerror(-size));
            exit(1);
        }
        int i = 0;
        len = read(sfd, buf, size);
       
		/*printf("%d",len);
        printf("%s",buf); 
        sleep(2);*/
        while (len < 0) {
            if (errno == EINTR)
                continue;
            perror("read()");
            break;
        }

        if (len == 0&& (tbf->token==20)) {
            break; // EOF
        }

        // Return unused tokens if any
        if (size - len > 0) {
            mytbf_returntoken(tbf, size - len);
        }

        while (len > 0) {
            ret = write(dfd, buf + pos, len);
            while (ret < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("write()");
                exit(1);
            }
            pos += ret;
            len -= ret;
        }
    }

    close(sfd);
    close(dfd);
    mytbf_destory(tbf);

    exit(0);
}
