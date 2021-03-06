#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "shared.h"

int consume_job(int consumer_no, PROC proc) {
    sleep(proc.compute_time);
    // printf("Consumer %d completes job %d\n", consumer_no, proc.job_id);
    return 0;
}

PROC remove_job(int consumer_no, SMT* shmseg) {
    PROC proc = dequeue(&(shmseg->pq));
    // printf("Consumer %d working on job %d for %ds\n", consumer_no, proc.job_id, proc.compute_time);

    printf("+--------------------------+\n");
    printf("| Job Removed From PQ      |\n");
    printf("+--------------+-----------+\n");
    printf("| Consumer     | %8d  |\n", consumer_no);
    printf("| Consumer pid | %8d  |\n", getpid());
    printf("| Producer     | %8d  |\n", proc.producer_no);
    printf("| Producer pid | %8d  |\n", proc.producer_pid);
    printf("| Priority     | %8d  |\n", proc.priority);
    printf("| Compute Time | %8d  |\n", proc.compute_time);
    printf("+--------------+-----------+\n\n");
    return proc;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    int shm_id;
    SMT *shmseg = create_shared_mem(&shm_id);
    int semid = create_semaphore_set();

    int item = 0;

    int i=atoi(argv[1]);
    int total_jobs = atoi(argv[2]);
    while(1) {
        int sleep_time = rand()%4;

        // printf("Consumer %d sleeps for %d\n\n", i, sleep_time);
        sleep(sleep_time);

        semop(semid, &downFull, 1);
        semop(semid, &downMutex, 1);

        PROC proc;
        int flag = 0;
        if(shmseg->job_completed < total_jobs) {
            proc = remove_job(i, shmseg);
            shmseg->job_completed++;
            // print_priority_queue(shmseg->pq);
            printf("+---------------+----------+\n");
            printf("| Job completed | %8d |\n", shmseg->job_completed);
            printf("+---------------+----------+\n\n");
            flag = 1;
        }
        semop(semid, &upMutex, 1);
        semop(semid, &upEmpty, 1);

        if(flag==1)
            consume_job(i, proc);
    } 
}