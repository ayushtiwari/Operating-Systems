#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define BLD   "\033[1m"
#define REG   "\033[0m"

#define NWORKERS 30
#define NBUF 100
#define PRODUCER_WORK 100
#define TIME_QUANTUM 0.0000500

int verbose=0;

int buf[NBUF];          // Buffer
int curr_size;          // Current size of buffer
int jobs_produced;      
int jobs_completed;

int switch_no=0;
int termination_no=0;

pthread_t scheduler_thread;
pthread_t status_thread;
pthread_mutex_t mutex;

clock_t t;
int producer_count = 0;
int consumer_count = 0;

// Worker type
typedef struct worker {
    int wid;
    enum {RUNNING, READY, TERMINATED, COMPLETED} state;
    enum {PRODUCER, CONSUMER} w_type;
    pthread_t *thread;
    struct worker *next;
} WORKER;

WORKER *running;            // Points to running worker
WORKER *ready_queue;        // Points to ready queue head

int enqueue(WORKER **queue, WORKER *w) {
    WORKER *q = *queue;
    if(q==0) {
        *queue = w;
        w->next = q;
    } else {
        while(q->next)
            q = q->next;
        q->next = w;
    }
    return 0;
}

WORKER* dequeue(WORKER **queue) {
    WORKER *w = *queue;
    if(w)
        *queue = (*queue)->next;
    w->next = NULL;
    return w;
}

int print_list(char *name, WORKER *p) {
    printf("%s = ", name);
    while(p) {
        printf("[%s %d]->", (p->w_type==PRODUCER)?"P":"C", p->wid);
        p = p->next;
    }
    printf("NULL\n");
    return 0;
}

void pause_handler(int sig, siginfo_t *siginfo, void *context) { 
    pause(); 
}
void resume_handler(int sig, siginfo_t *siginfo, void *context) { 
    usleep(10000);
    t = clock();
}

double time_elapsed() {
    clock_t diff = clock()-t;
    return ((double)diff)/CLOCKS_PER_SEC;
}

void *producer_action(void *arg) {

    pause();
    int i=0;
    while(1) {
 
        while(pthread_mutex_trylock(&mutex)!=0) {
            continue;
            usleep(100);
        }
        // Critical Section
        if(curr_size < NBUF && i < PRODUCER_WORK) {
            buf[curr_size++] = rand()%100000;
            jobs_produced++;
            i++;
        }

        if(i >= PRODUCER_WORK) {
            running->state = COMPLETED;
        }

        clock_t diff = clock()-t;
        double time_taken = ((double)diff)/CLOCKS_PER_SEC;
        if(curr_size==NBUF || time_taken > TIME_QUANTUM) {
            pthread_mutex_unlock(&mutex);
            pthread_kill(scheduler_thread, SIGUSR2);
            pause();
            continue;
        }

        // Produce items
        pthread_mutex_unlock(&mutex);
    }
}

void *consumer_action(void *arg) {

    pause();
    while(1) {
        while(pthread_mutex_trylock(&mutex)!=0) {
            continue;
            usleep(100);
        }
        // Critical Section
        if(curr_size > 0) {
            curr_size--;
            jobs_completed++;
        }
        clock_t diff = clock()-t;
        double time_taken = ((double)diff)/CLOCKS_PER_SEC;
        if(curr_size==0 || time_taken > TIME_QUANTUM) {
            pthread_mutex_unlock(&mutex);
            pthread_kill(scheduler_thread, SIGUSR2);
            pause();
            continue;
        }
        
        pthread_mutex_unlock(&mutex);
        // Consume item
    }
}

WORKER *new_worker(enum {PRODUCER, CONSUMER} w_type, int wid) {
    WORKER *w = (WORKER*)malloc(sizeof(WORKER));
    w->next=NULL;
    w->state = READY;
    w->w_type = w_type;
    w->thread = NULL;
    w->wid = wid;
    return w;
}

void *status_action() {

    pause();

    while(1) {
        
        if(running->state == RUNNING) {
        if(verbose
    ){
        printf(" %s%sswitch %d\n", BLD, KYEL, switch_no);
        printf("%s----------------+-----------------\n", KNRM);
        }
        printf(" %s%srunning        %s| %s%s[%s %d]     \n", BLD, KCYN, KNRM, BLD, KCYN, (running->w_type==PRODUCER)?"P":"C",running->wid);
        }
        else{
        if(verbose
    ){
        printf(" %s%stermination %d\n", BLD, KYEL, termination_no);
        printf("%s----------------+-----------------\n", KNRM);
        }
        printf(" %s%sterminate      %s| %s%s[%s %d]     \n", BLD, KCYN, KNRM, BLD, KCYN, (running->w_type==PRODUCER)?"P":"C", running->wid);
        }
        printf(" %s%sbuffer         %s| %s%s%d/%d       \n", BLD, KCYN, KNRM, BLD, KCYN, curr_size, NBUF);
        if(verbose
    ) {
        printf("%s----------------+-----------------\n", KNRM);
        printf(" %s%sjobs-completed %s| %s%s%d          \n", BLD, KGRN, KNRM, BLD, KGRN, jobs_completed);
        printf(" %s%sjobs-produced  %s| %s%s%d/%d       \n", BLD, KMAG, KNRM, BLD, KMAG, jobs_produced, producer_count*PRODUCER_WORK);
        }
        printf("%s==================================\n", KNRM);
        pthread_kill(scheduler_thread, SIGUSR2);
        pause();
    }
}

void *scheduler_action() {

    while(1) {
        if(ready_queue==NULL) return 0;
        running = dequeue(&ready_queue);
        
        if(running->thread == NULL) {
            running->thread = (pthread_t*)malloc(sizeof(pthread_t));
            if(running->w_type==PRODUCER) {
                pthread_create(running->thread, NULL, producer_action, (void*)running->wid);
            } else {
                pthread_create(running->thread, NULL, consumer_action, (void*)running->wid);
            }
            usleep(20000);   
        }

        switch_no++;
        running->state = RUNNING;
        pthread_kill(status_thread, SIGUSR2);               // Start the Status Thread
        pause();

        pthread_kill(*(running->thread), SIGUSR2);          // Start the current worker thread
        pause();

        pthread_mutex_lock(&mutex);
        
        pthread_kill(*(running->thread), SIGUSR1);          // Stop the current worker thread
        
        // Set status
	    if(running->state == COMPLETED || jobs_completed >= producer_count*PRODUCER_WORK) {
            termination_no++;
            running->state = TERMINATED;
            pthread_kill(status_thread, SIGUSR2);
            pause();

            pthread_cancel(*(running->thread));
        } else 
            running->state = READY;
        
        pthread_mutex_unlock(&mutex);

        // Enqueue the worker
        if(running->state == READY)
            enqueue(&ready_queue, running);
    }

    pthread_exit(0);
}

void init() {
    running = NULL;
    ready_queue = NULL;
    memset(buf, 0, sizeof(buf));
    jobs_completed = curr_size = jobs_produced = 0;
    pthread_mutex_init(&mutex, NULL);
}

int main(int argc, char *argv[]) {

    char c;
    while((c=getopt(argc, argv, "v"))!=-1){
        switch(c) {
            case 'v':
                verbose=1;
                break;
        }
    }
    // Install signal handlers
    struct sigaction pause_act;
    memset(&pause_act, 0, sizeof(pause_act));
    pause_act.sa_sigaction = &pause_handler;
    pause_act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &pause_act, NULL);

    struct sigaction resume_act;
    memset(&resume_act, 0, sizeof(resume_act));
    resume_act.sa_sigaction = &resume_handler;
    resume_act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &resume_act, NULL);

    srand(time(NULL));

    // Init global variables
    init();

    // Create workers, fill readyqueue
    WORKER *w[NWORKERS];
    int i;	
    for(i=0; i<NWORKERS; i++) {
        if(rand()%2)
            w[i] = new_worker(PRODUCER, producer_count++);
        else 
            w[i] = new_worker(CONSUMER, consumer_count++);

        enqueue(&ready_queue, w[i]);
    }

    printf("------------------------------\n");
    printf(" %stotal-workers: %s%d\n", KYEL, KCYN, NWORKERS);
    printf(" %sproducers: %s%d, %sconsumers: %s%d\n", KYEL, KCYN, producer_count, KYEL, KCYN, consumer_count);
    printf(" %sbuffer-size-limit: %s%d\n", KYEL, KCYN, NBUF);
    printf(" %stime-quantum: %s%lfs\n", KYEL, KCYN, TIME_QUANTUM);
    printf("%s------------------------------\n", KNRM);
    print_list("ready_queue", ready_queue);    
    printf("------------------------------\n");
    printf("initiation complete! Press enter to continue ");
    getchar();
    // Begin scheduling
    pthread_create(&scheduler_thread, NULL, scheduler_action, NULL);
    // Begin monitoring
    pthread_create(&status_thread, NULL, status_action, NULL);

    pthread_join(scheduler_thread, NULL);
    printf(" FINISHED\n");
    printf("----------------------------------\n");
    printf(" Summary\n");
    printf("----------------------------------\n");
    printf(" jobs-completed         | %d\n", jobs_completed);
    printf(" jobs-produced          | %d\n", jobs_produced);
    printf(" total-context-switches | %d\n", switch_no);
    printf("----------------------------------\n");
    pthread_cancel(status_thread);

    return 0;
}
