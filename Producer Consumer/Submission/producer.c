#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

#include "shared.h"
#include "queue.h"

int insert_job(int producer_no, PROC proc, SMT *shmseg) {
    enqueue(&(shmseg->pq), proc);
    // printf("Producer %d inserts job %d\n", producer_no, proc.job_id);

    printf("======================\n");
    printf("New Job Created!\n");
    printf("----------------------\n");
    printf("Producer: %d\n", proc.producer_no);
    printf("Producer pid: %d\n", proc.producer_pid);
    printf("Priority: %d\n", proc.priority);
    printf("Compute Time: %d\n", proc.compute_time);
    printf("======================\n\n");

    return 0;
}

PROC produce_job(int producer_no, int counter) {
    PROC proc;
    proc.producer_no = producer_no;
    proc.job_id = counter;
    proc.priority = rand()%20;
    proc.producer_pid = getpid();
    proc.compute_time = 1 + rand()%4;

    // printf("Producer %d creates job %d\n", producer_no, proc.job_id);

    return proc;
}


int main(int argc, const char *argv[])
{   
    srand(time(NULL));
    int shm_id;
    SMT *shmseg = create_shared_mem(&shm_id);
    // init_shared_mem(shmseg);
    int semid = create_semaphore_set();

    if(argc < 3) exit(1);
    
    int i=atoi(argv[1]);
    int total_jobs = atoi(argv[2]);
    int counter=0;
    while(1) {
        PROC proc = produce_job(i, counter);
        int sleep_time = rand()%4;
        // printf("Producer %d sleeps for %ds\n", i, sleep_time);
        sleep(sleep_time);

        semop(semid, &downEmpty, 1);
        semop(semid, &downMutex, 1);

        if(shmseg->job_created < total_jobs) {
            insert_job(i, proc, shmseg);
            shmseg->job_created++;
            print_priority_queue(shmseg->pq);
            printf("Job completed: %d, Job created: %d\n\n", shmseg->job_completed, shmseg->job_created);
        }

        semop(semid, &upMutex, 1);
        semop(semid, &upFull, 1);
        
        counter++;
    }
 
}