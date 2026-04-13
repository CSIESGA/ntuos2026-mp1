#include "kernel/types.h"
#include "user/user.h"
#include "user/threads.h"

#define NULL 0
static struct thread *g_t4 = 0;

void s1(int signo)
{
    int i = 0;
    while(1) {
        if(signo) printf("handler 1: %d\n", i);
        else printf("handler 1: %d\n", i+1);
        i++;
        if (i == 3) {
            return;
        }
        thread_yield();
    }    
}
void f9(void *arg)
{
    int i = 9000;

    while(1) {
        printf("thread 9: %d\n", i++);
        if (i == 9002) {
            thread_exit();
        }
        thread_yield();
    }
}
void f8(void *arg)
{
    int i = 800;

    while(1) {
        printf("thread 8: %d\n", i++);
        if (i == 803) {
            printf("thread 8: join thread 4\n");
            thread_join(g_t4);
        }
        if (i == 806) {
            thread_exit();
        }
        thread_yield();
    }
}

void f7(void *arg)
{
    int i = 0;

    while(1) {
        printf("thread 7: %d\n", i++);
        if (i == 3) {
            thread_exit();
        }
        thread_yield();
    }
}

void f6(void *arg)
{
    int i = 0;

    while(1) {
        printf("thread 6: %d\n", i++);
        if (i == 2) {
            
            thread_exit();
        }
        thread_yield();
    }
}

void f5(void *arg)
{
    int i = 0;

    while(1) {
        printf("thread 5: %d\n", i++);
        if (i == 3) {
            thread_exit();
        }
        thread_yield();
    }
}

void f4(void *arg)
{
    int i = 0;

    while(1) {
        printf("thread 4: %d\n", i++);
        g_t4 = get_current_thread();
        if(i == 1) {
            printf("thread %d: suspending\n", g_t4->ID);
            thread_suspend(g_t4);
        }    
        if(i == 3) {
            struct thread *t9 = thread_create(f9, NULL);
            thread_add_runqueue(t9);
            printf("thread 4: join thread 9\n");
            thread_join(t9);
        }
        if (i == 5) {
            thread_exit();
        }
        thread_yield();
    }
}

void f3(void *arg)
{
    int i = 0;

    while(1) {
        printf("thread 3: %d\n", i++);
        if (i == 3) {
            struct thread *t6 = thread_create(f6, NULL);
            thread_add_runqueue(t6);
            struct thread *t7 = thread_create(f7, NULL);
            thread_add_runqueue(t7);
            thread_kill(t6, 0);
            thread_kill(t7, 1);
            thread_exit();
        }
        thread_yield();
    }
}

void f2(void *arg)
{
    int i = 0;

    while(1) {
        printf("thread 2: %d\n", i++);
        if (i == 2) {
            struct thread *t4 = thread_create(f4, NULL);
            thread_add_runqueue(t4);
            struct thread *t5 = thread_create(f5, NULL);
            thread_add_runqueue(t5);
            struct thread *t8 = thread_create(f8, NULL);
            thread_add_runqueue(t8);
            thread_kill(t4, 0);
            thread_kill(t5, 1);
            thread_exit();
        }
        thread_yield();
    }
}

void f1(void *arg)
{
    int i = 0;

    thread_register_handler(0, s1);
    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(f3, NULL);
    thread_add_runqueue(t3);
    
    while(1) {
        printf("thread 1: %d\n", i++);
        if (i == 10) {
            printf("thread %d: resuming\n", g_t4->ID);
            thread_resume(g_t4);
            printf("thread 1: join thread 4\n");
            thread_join(g_t4);
        }
        if(i == 15) {
            thread_exit();
        }
        thread_yield();
    }
}

int main(int argc, char **argv)
{
    printf("mp1-part2-3\n");
    struct thread *t1 = thread_create(f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("\nexited\n");
    exit(0);
}