#ifndef MYTBF_H
#define MYTBF_H

#define TBFSIZE 1024
typedef void mytbf_t;


static struct mytbf_st *job[TBFSIZE];  // *表示指针数组

int mytbf_fetchtoken(mytbf_t *, int);  /*在函数声明中省略形参名字是合法的，通常用于简化代码的书写或隐藏实现细节*/
int mytbf_returntoken(mytbf_t *, int );
int mytbf_destory(mytbf_t *);

#endif
