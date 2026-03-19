// #include "kernel/types.h"
// #include "user/setjmp.h"
// #include "user/threads.h"
// #include "user/user.h"
// #define NULL 0

// static struct thread* current_thread = NULL;
// static int id = 1;

// //the below 2 jmp buffer will be used for main function and thread context switching
// static jmp_buf env_st;
// static jmp_buf env_tmp;

// struct thread *get_current_thread() {
//     return current_thread;
// }

// struct thread *thread_create(void (*f)(void *), void *arg){
//     struct thread *t = (struct thread*) malloc(sizeof(struct thread));
//     unsigned long new_stack_p;
//     unsigned long new_stack;
    
//     new_stack = (unsigned long) malloc(sizeof(unsigned long)*0x100);
//     new_stack_p = new_stack + 0x100*8 - 0x2*8;
    
//     t->fp = f;
//     t->arg = arg;
//     t->ID  = id;
//     t->buf_set = 0;
//     t->stack = (void*) new_stack;  //points to the beginning of allocated stack memory for the thread.
//     t->stack_p = (void*) new_stack_p;  //points to the current execution part of the thread.
//     id++;
//     // part 2
//     t->sig_handler[0] = NULL_FUNC;
//     t->sig_handler[1] = NULL_FUNC;
//     t->signo = -1;
//     t->handler_buf_set = 0;
    
//     t->suspended = 0;      
//     t->join_blocked = 0;   
    
//     t->exited = 0;      
//     t->waiter_list = NULL; 
//     t->next_waiter = NULL; 
//     return t;
// }

// void thread_add_runqueue(struct thread *t){
//     if(current_thread == NULL){
//         //TODO
//     }
//     else{
//         //TODO
//     }
// }

// void thread_yield(void){
//     //TODO
// }

// void dispatch(void){
//     //TO DO   
// }

// //schedule will follow the rule of FIFO
// void schedule(void){
//     current_thread = current_thread->next;
    
//     //TODO
//     while(current_thread->suspended || current_thread->join_blocked) {
//     };
// }

// void thread_exit(void) {
//     struct thread *w = current_thread->waiter_list;
//     struct thread *ptr = current_thread;
//     //TODO (Part2)

//     if (current_thread->next != current_thread) {
//        //TODO
//     } else {
//         //TODO
//     }
// }

// void thread_start_threading(void){
//     //TODO
// }

// void thread_register_handler(int signo, void (*handler)(int)){
//     current_thread->sig_handler[signo] = handler;
// }

// void thread_kill(struct thread *t, int signo){
//     //TODO
// }

// void thread_suspend(struct thread *t) {
//     //TODO
// }

// void thread_resume(struct thread *t) {
//     //TODO
// }

// void thread_join(struct thread *t) {
//     //TODO
// }
#include "kernel/types.h"
#include "user/setjmp.h"
#include "user/threads.h"
#include "user/user.h"

#define NULL 0

static struct thread *current_thread = NULL;
static int id = 1;

// env_st: return-to-main context
// env_tmp: temporary bootstrap context for a brand-new thread / handler
static jmp_buf env_st;
static jmp_buf env_tmp;

static void wake_waiters(struct thread *t) {
    struct thread *w = t->waiter_list;
    while (w != NULL) {
        struct thread *next_w = w->next_waiter;
        w->join_blocked = 0;
        // w->suspended = 0;
        w->next_waiter = NULL;
        w = next_w;
    }
    t->waiter_list = NULL;
}

struct thread *get_current_thread() {
    return current_thread;
}

struct thread *thread_create(void (*f)(void *), void *arg) {
    struct thread *t = (struct thread *)malloc(sizeof(struct thread));
    unsigned long new_stack;
    unsigned long new_stack_p;

    new_stack = (unsigned long)malloc(sizeof(unsigned long) * 0x100);
    new_stack_p = new_stack + 0x100 * 8 - 0x2 * 8; // 8-byte aligned

    t->fp = f;
    t->arg = arg;
    t->stack = (void *)new_stack;
    t->stack_p = (void *)new_stack_p;

    t->buf_set = 0;
    t->ID = id++;
    t->previous = NULL;
    t->next = NULL;

    // join
    t->waiter_list = NULL;
    t->next_waiter = NULL;
    t->exited = 0;

    // signal
    t->sig_handler[0] = NULL_FUNC;
    t->sig_handler[1] = NULL_FUNC;
    t->signo = -1;
    t->handler_buf_set = 0;
    t->handling_signal = 0;

    // status
    t->join_blocked = 0;
    t->suspended = 0;

    return t;
}

void thread_add_runqueue(struct thread *t) {
    if (t == NULL)
        return;

    if (current_thread == NULL) {
        current_thread = t;
        t->next = t;
        t->previous = t;
    } else {
        struct thread *tail = current_thread->previous;

        tail->next = t;
        t->previous = tail;
        t->next = current_thread;
        current_thread->previous = t;

        // child inherits signal handlers from current thread
        t->sig_handler[0] = current_thread->sig_handler[0];
        t->sig_handler[1] = current_thread->sig_handler[1];
    }
}

// FIFO scheduler: move to next runnable thread
void schedule(void) {
    struct thread *start;

    if (current_thread == NULL)
        return;

    start = current_thread;
    do {
        current_thread = current_thread->next;
    } while ((current_thread->suspended || current_thread->join_blocked) && current_thread != start);
}

void thread_yield(void) {
    if (current_thread == NULL)
        return;

    // if currently executing signal handler, save handler context
    if (current_thread->handling_signal) {
        if (setjmp(current_thread->handler_env) == 0) {
            current_thread->handler_buf_set = 1;
            schedule();
            dispatch();
        }
    } else {
        if (setjmp(current_thread->env) == 0) {
            current_thread->buf_set = 1;
            schedule();
            dispatch();
        }
    }
}

void dispatch(void) {
    unsigned long resume_sp;

    if (current_thread == NULL) {
        longjmp(env_st, 1);
    }

    // skip blocked/suspended threads
    while (current_thread->suspended || current_thread->join_blocked) {
        schedule();
    }

    // pending signal
    if (current_thread->signo != -1) {
        int signo = current_thread->signo;
        void (*handler)(int) = current_thread->sig_handler[signo];

        // no handler => kill thread at next dispatch
        if (handler == NULL_FUNC) {
            thread_exit();
            return;
        }

        current_thread->handling_signal = 1;

        // resume previously-yielded handler
        if (current_thread->handler_buf_set) {
            longjmp(current_thread->handler_env, 1);
        }

        // first time entering handler
        // if thread already ran before, handler should start from saved thread sp
        // otherwise start from thread's initial stack top
        if (current_thread->buf_set)
            resume_sp = current_thread->env[0].sp;
        else
            resume_sp = (unsigned long)current_thread->stack_p;

        if (setjmp(env_tmp) == 0) {
            env_tmp[0].sp = resume_sp;
            longjmp(env_tmp, 1);
        } else {
            handler(signo);

            // handler returns => clear signal state, continue original thread
            current_thread->handling_signal = 0;
            current_thread->handler_buf_set = 0;
            current_thread->signo = -1;

            if (current_thread->buf_set) {
                longjmp(current_thread->env, 1);
            } else {
                current_thread->fp(current_thread->arg);
                thread_exit();
            }
        }
        return;
    }

    // normal resume
    if (current_thread->buf_set) {
        longjmp(current_thread->env, 1);
    }

    // first time running this thread
    if (setjmp(env_tmp) == 0) {
        env_tmp[0].sp = (unsigned long)current_thread->stack_p;
        longjmp(env_tmp, 1);
    } else {
        current_thread->fp(current_thread->arg);
        thread_exit();
    }
}

void thread_exit(void) {
    struct thread *ptr;
    struct thread *next;

    if (current_thread == NULL) {
        longjmp(env_st, 1);
    }

    ptr = current_thread;
    ptr->exited = 1;

    // wake up threads waiting on me
    wake_waiters(ptr);

    if (ptr->next != ptr) {
        next = ptr->next;

        ptr->previous->next = ptr->next;
        ptr->next->previous = ptr->previous;
        current_thread = next;

        if (ptr->stack != NULL) {
            free(ptr->stack);
            ptr->stack = NULL;
            ptr->stack_p = NULL;
        }
        free(ptr);
        
        // NOTE:
        // I intentionally do NOT free(ptr) here, because thread_join(t)
        // may still need to check t->exited after the target has exited.
        dispatch();
    } else {
        current_thread = NULL;

        if (ptr->stack != NULL) {
            free(ptr->stack);
            ptr->stack = NULL;
            ptr->stack_p = NULL;
        }
        free(ptr);

        longjmp(env_st, 1);
    }
}

void thread_start_threading(void) {
    if (setjmp(env_st) == 0) {
        dispatch();
    }
}

void thread_register_handler(int signo, void (*handler)(int)) {
    current_thread->sig_handler[signo] = handler;
}

void thread_kill(struct thread *t, int signo) {
    if (t == NULL || t->exited)
        return;
    t->signo = signo;
}

void thread_suspend(struct thread *t) {
    if (t == NULL || t->exited)
        return;

    t->suspended = 1;

    if (t == current_thread) {
        thread_yield();
    }
}

void thread_resume(struct thread *t) {
    if (t == NULL || t->exited)
        return;

    t->suspended = 0;
}

void thread_join(struct thread *t) {
    struct thread *p;

    if (t == NULL || t == current_thread || t->exited)
        return;

    if (t->waiter_list == NULL) {
        t->waiter_list = current_thread;
    } else {
        p = t->waiter_list;
        while (p->next_waiter != NULL) {
            p = p->next_waiter;
        }
        p->next_waiter = current_thread;
    }

    current_thread->next_waiter = NULL;
    current_thread->join_blocked = 1;
    thread_yield();
}