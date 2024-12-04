#ifndef MYSEM_H_
#define MYSEM_H_

void *mysem_init(int val);
int mysem_add(void *ptr, int num);
int mysem_sub(void *ptr,int num);
int mysem_destory(void *ptr);
#endif