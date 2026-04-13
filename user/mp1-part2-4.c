#include "kernel/types.h"
#include "user/user.h"
#include "user/threads.h"

#define NULL 0
static struct thread *g_t5=0;

void s7(int signo)
{
    int i = 0;
    while(1) {
        if(signo) printf("handler 7: %d\n", 7*i);
        else printf("handler 7: %d\n", 7*i+1);
        i++;
        if (i == 3) {
            return;
        }
        thread_yield();
    }    
}

void s6(int signo)
{
    int i = 0;
    while(1) {
        if(signo) printf("handler 6: %d\n", 6*i);
        else printf("handler 6: %d\n", 6*i+1);
        i++;
        if (i == 3) {
            return;
        }
        thread_yield();
    }    
}

void s5(int signo)
{
    int i = 0;
    while(1) {
        if(signo) printf("handler 5: %d\n", 5*i);
        else printf("handler 5: %d\n", 5*i+1);
        i++;
        if (i == 3) {
            return;
        }
        thread_yield();
    }    
}

void s4(int signo)
{
    int i = 0;
    while(1) {
        if(signo) printf("handler 4: %d\n", 4*i);
        else printf("handler 4: %d\n", 4*i+1);
        i++;
        if (i == 3) {
            return;
        }
        thread_yield();
    }    
}

void s3(int signo)
{
    int i = 0;
    while(1) {
        if(signo) printf("handler 3: %d\n", 3*i);
        else printf("handler 3: %d\n", 3*i+1);
        i++;
        if (i == 3) {
            return;
        }
        thread_yield();
    }    
}

void s2(int signo)
{
    int i = 0;
    while(1) {
        if(signo) printf("handler 2: %d\n", 2*i);
        else printf("handler 2: %d\n", 2*i+1);
        i++;
        if (i == 3) {
            return;
        }
        thread_yield();
    }    
}

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

void f15(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 15: %d\n", i++);
        if (i == 15) {
            thread_exit();
        }
        thread_yield();
    }
}

void f14(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 14: %d\n", i++);
        if (i == 5) {
            thread_join(g_t5);
        }
        if (i == 14) {
            thread_exit();
        }
        thread_yield();
    }
}

void f13(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 13: %d\n", i++);
        if (i == 13) {
            thread_exit();
        }
        thread_yield();
    }
}

void f12(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 12: %d\n", i++);
        if (i == 12) {
            thread_exit();
        }
        thread_yield();
    }
}

void f11(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 11: %d\n", i++);
        if (i == 11) {
            if(g_t5){
                printf("thread %d: resuming\n", g_t5->ID);    
                thread_resume(g_t5);
            }
            thread_exit();
        }
        thread_yield();
    }
}

void f10(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 10: %d\n", i++);
        if (i == 10) {
            thread_exit();
        }
        thread_yield();
    }
}

void f9(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 9: %d\n", i++);
        if (i == 9) {
            thread_exit();
        }
        thread_yield();
    }
}

void f8(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 8: %d\n", i++);
        if (i == 8) {
            thread_exit();
        }
        thread_yield();
    }
}

void f7(void *arg)
{
    int i = 0;

    thread_register_handler(0, s7);
    thread_register_handler(1, s7);

    struct thread *t14 = thread_create(f14, NULL);
    thread_add_runqueue(t14);
    struct thread *t15 = thread_create(f15, NULL);
    thread_add_runqueue(t15);

    while(1) {
        printf("thread 7: %d\n", i++);
        if(i == 1) {
            thread_kill(t14, 0);
            thread_kill(t15, 1);
        }
        if (i == 7) {
            thread_exit();
        }
        thread_yield();
    }
}

void f6(void *arg)
{
    int i = 0;

    thread_register_handler(0, s6);
    thread_register_handler(1, s6);

    struct thread *t12 = thread_create(f12, NULL);
    thread_add_runqueue(t12);
    struct thread *t13 = thread_create(f13, NULL);
    thread_add_runqueue(t13);

    while(1) {
        printf("thread 6: %d\n", i++);
        if(i == 1) {
            thread_kill(t12, 0);
            thread_kill(t13, 1);
        }
        if (i == 6) {
            thread_exit();
        }
        thread_yield();
    }
}

void f5(void *arg)
{
    int i = 0;

    thread_register_handler(0, s5);
    thread_register_handler(1, s5);

    struct thread *t10 = thread_create(f10, NULL);
    thread_add_runqueue(t10);
    struct thread *t11 = thread_create(f11, NULL);
    thread_add_runqueue(t11);

    while(1) {
        printf("thread 5: %d\n", i++);
        if(i == 1) {
            thread_kill(t10, 0);
            thread_kill(t11, 1);
        }
        if(i == 3){
            g_t5 = get_current_thread();
            printf("thread %d: suspending\n", g_t5->ID);             // "thread 1: suspending"
            thread_suspend(g_t5);  // g_t1 points to the same thread
        }
        if (i == 5) {
            thread_exit();
        }
        thread_yield();
    }
}

void f4(void *arg)
{
    int i = 0;

    thread_register_handler(0, s4);
    thread_register_handler(1, s4);

    struct thread *t8 = thread_create(f8, NULL);
    thread_add_runqueue(t8);
    struct thread *t9 = thread_create(f9, NULL);
    thread_add_runqueue(t9);

    while(1) {
        printf("thread 4: %d\n", i++);
        if(i == 1) {
            thread_kill(t8, 0);
            thread_kill(t9, 1);
        }
        if(i == 2) {
            thread_join(t8);
        }
        if (i == 4) {
            thread_exit();
        }
        thread_yield();
    }
}

void f3(void *arg)
{
    int i = 0;

    thread_register_handler(0, s3);
    thread_register_handler(1, s3);

    struct thread *t6 = thread_create(f6, NULL);
    thread_add_runqueue(t6);
    struct thread *t7 = thread_create(f7, NULL);
    thread_add_runqueue(t7);

    while(1) {
        printf("thread 3: %d\n", i++);
        if(i == 1) {
            thread_kill(t6, 0);
            thread_kill(t7, 1);
        }
        if (i == 3) {
            thread_exit();
        }
        thread_yield();
    }
}

void f2(void *arg)
{
    int i = 0;

    thread_register_handler(0, s2);
    thread_register_handler(1, s2);

    struct thread *t4 = thread_create(f4, NULL);
    thread_add_runqueue(t4);
    struct thread *t5 = thread_create(f5, NULL);
    thread_add_runqueue(t5);

    while(1) {
        printf("thread 2: %d\n", i++);
        if(i == 1) {
            thread_kill(t4, 0);
            thread_kill(t5, 1);
        }
        if (i == 2) {
            thread_join(t4);
        }
        if (i == 5) {
            thread_exit();
        }
        thread_yield();
    }
}

void f1(void *arg)
{
    int i = 0;

    thread_register_handler(0, s1);
    thread_register_handler(1, s1);

    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(f3, NULL);
    thread_add_runqueue(t3);
    
    while(1) {
        printf("thread 1: %d\n", i++);
        if (i == 1) {
            thread_kill(t2, 0);
            thread_kill(t3, 1);
        }
        if(i == 3) {
            thread_join(t3);
            thread_join(t2);
        }
        
        if(i == 8){
            thread_exit();
        }
        thread_yield();
    }
}

int main(int argc, char **argv)
{
    printf("mp1-part2-4\n");
    struct thread *t1 = thread_create(f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("\nexited\n");
    exit(0);
}