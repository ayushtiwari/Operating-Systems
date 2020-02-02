#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "shared.h"
#include "queue.h"

int main() {

    int n_producers = 5, n_consumers = 4, total_jobs = 12;

    printf("Enter no. of producers: ");
    scanf("%d", &n_producers);

    printf("Enter no. of consumers: ");
    scanf("%d", &n_consumers);

    printf("Enter no. of jobs: ");
    scanf("%d", &total_jobs);

    int all_pid[1024];
    int k_p = 0;

    pid_t child_pid, wpid;
    int status = 0;

    int shm_id;
    SMT *shmseg = create_shared_mem(&shm_id);

    printf("shm_id = %d\n", shm_id);
    init_shared_mem(shmseg);
    int semid = create_semaphore_set();

    char str[5];
    char tot_jobs[5];

    sprintf(tot_jobs, "%d", total_jobs);
    
    for(int i=0; i<n_producers; i++) {
        sprintf(str, "%d", i);
        char *argv[] = {"producer", str, tot_jobs, NULL};
        int pid = fork();
        if(!pid)
            execv("./producer", argv);
        else
            all_pid[k_p++] = pid;
    }

    
    for(int i=0; i<n_consumers; i++) {
        sprintf(str, "%d", i);
        char *argv[] = {"consumer", str, tot_jobs, NULL};
        int pid = fork();
        if(!pid)
            execv("./consumer", argv);
        else
            all_pid[k_p++] = pid;
    }

    time_t start_t, end_t;
    double diff_t;
    time(&start_t);

    union semun arg;

    while(1) {
        sleep(1);
        semop(semid, &downMutex, 1);
        if(shmseg->job_created == total_jobs && shmseg->job_completed == total_jobs) {
            for(int i=0; i<k_p; i++) {
                kill(all_pid[i], SIGTERM);
            }
            time(&end_t);
            diff_t = difftime(end_t, start_t);
            printf("Execution time = %0.4fs\n", diff_t);
            semctl(semid, 0, IPC_RMID, arg);
            shmctl(shm_id, IPC_RMID, NULL);
            exit(0);
        }
        semop(semid, &upMutex, 1);
    }

}