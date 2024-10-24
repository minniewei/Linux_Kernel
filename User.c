#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h> /* Definition of SYS_* constants */
#include <unistd.h>

#define get_call 449

extern void *func1(void *);
extern void *func2(void *);
extern void *func3(void *);
extern int main();

struct data_
{
    int id;
    char name[16];
};

typedef struct data_ sdata;
static __thread sdata tx; // thread local variable

int a = 123; // global variable
int c;

void hello(int pid)
{
    int b = 10;  // local varialbe
    b = b + pid; // global variable
    char *str = (char *)malloc(60 * sizeof(char));
    strcpy(str, "heap_str");
    printf("In thread %d \nthe value of global variable a is %d, the offset of the logical address of  a is %p, ", pid, a, &a);
    printf("the physical address of global variable a is %p\n", (void *)syscall(get_call, &a));
    // local variable
    printf("the value of local varialbe b is %d, the offset of the logical address of b is %p, ", b, &b);
    printf("the physical address of local variable b is %p\n", (void *)syscall(get_call, &b));
    // thread local variable
    printf("the value of uninitialized data c (bss) is %d, the offset of the logical address of c is %p, ", c, &c);
    printf("the physical address of uninitialized data c (bss) is %p\n", (void *)syscall(get_call, &c));
    // bss segments
    printf("the offset of the logical address of str (heap) is %p\n", str);
    printf("the physical address of str (heap) is %p\n", (void *)syscall(get_call, str));
    // heap
    printf("the offset of the logical address of thread local varialbe tx is %p, ", &tx);
    printf("the physical address of thread local variable tx is %p\n", (void *)syscall(get_call, &tx));
    // function
    printf("the offset of the logical address of function hello is %p, ", hello);
    printf("the physical address of function hello is %p\n", (void *)syscall(get_call, hello));

    printf("the offset of the logical address of function func1 is %p, ", func1);
    printf("the physical address of function func1 is %p\n", (void *)syscall(get_call, func1));

    printf("the offset of the logical address of function func2 is %p, ", func2);
    printf("the physical address of function func2 is %p\n", (void *)syscall(get_call, func2));

    printf("the offset of the logical address of function func3 is %p, ", func3);
    printf("the physical address of function func3 is %p\n", (void *)syscall(get_call, func3));

    printf("the offset of the logical address of function main is %p, ", main);
    printf("the physical address of function main is %p\n", (void *)syscall(get_call, main));
    // library function
    printf("the offset of the logical address of library function printf is %p, ", printf);
    printf("the physical address of library function printf is %p\n", (void *)syscall(get_call, printf));
    printf("====================================================================================================================\n");
    free(str);
}

void *func1(void *arg)
{
    char *p = (char *)arg;
    int pid;
    pid = syscall(__NR_gettid);
    tx.id = pid;
    strcpy(tx.name, p);
    printf("I am thread with ID %d executing func1().\n", pid);
    hello(pid);

    while (1)
    {
        printf("(%d)(%s)\n", tx.id, tx.name);
        sleep(2);
    }
}

void *func2(void *arg)
{
    char *p = (char *)arg;
    int pid;
    pid = syscall(__NR_gettid);
    tx.id = pid;
    strcpy(tx.name, p);
    printf("I am thread with ID %d executing func2().\n", pid);
    hello(pid);

    while (1)
    {
        printf("(%d)(%s)\n", tx.id, tx.name);
        sleep(2);
    }
}

void *func3(void *arg)
{
    char *p = (char *)arg;
    int pid;
    pid = syscall(__NR_gettid);
    tx.id = pid;
    strcpy(tx.name, p);
    printf("I am thread with ID %d executing func3().\n", pid);
    hello(pid);
    while (1)
    {
        // printf("(%d)(%s)\n",tx.id,tx.name) ;
        sleep(2);
    }
}
int main()
{
    pthread_t id[3];
    char p[3][16];
    strcpy(p[0], "Thread1");
    pthread_create(&id[0], NULL, func1, (void *)p[0]);
    sleep(1);

    strcpy(p[1], "Thread2");
    pthread_create(&id[1], NULL, func2, (void *)p[1]);
    sleep(1);

    strcpy(p[2], "Thread3");
    pthread_create(&id[2], NULL, func3, (void *)p[2]);
    sleep(1);

    int pid;
    pid = syscall(__NR_gettid);
    tx.id = pid;
    strcpy(tx.name, "MAIN");
    printf("I am main thread with ID %d.\n", pid);
    hello(pid);
    sleep(1);
    while (1)
    {
        printf("(%d)(%s)\n", tx.id, tx.name);
        sleep(50);
    }
}