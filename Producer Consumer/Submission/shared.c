#include "shared.h"

int init_shared_mem(SMT* shmseg) {
    shmseg->job_completed = 0;
    shmseg->job_created = 0;
    init_queue(&shmseg->pq);

    return 0;
}

/**
 * returns a pointer to a shared memory buffer that the
 * producer can write to.
 */
SMT *create_shared_mem(int *shm_id) {
//   int *shmaddr = 0; /* buffer address */
  key_t key = SHM_KEY; /* use key to access a shared memory segment */
  
  int shmid = shmget(key, sizeof(SMT), IPC_CREAT | SHM_R | SHM_W); /* give create, read and write access */
  if (errno > 0) {
    perror("shmget failed to create shared memory segment");
    exit (EXIT_FAILURE);
  }

  SMT *shmseg = (SMT*)shmat(shmid, NULL, 0);
  if (errno > 0) {
    perror ("shmseg failed to attach to shared memory segment");
    exit (EXIT_FAILURE);
  }

  *shm_id = shmid;
  // clean out garbage memory in shared memory
  return shmseg;
}

/**
 * create FULL and EMPTY semaphores
 */
int create_semaphore_set() {
  key_t key = ftok(SEM_KEY, 'E');
  
  int semid = semget(key, NSEM_SIZE, 0600 | IPC_CREAT);
  if (errno > 0) {
    perror("failed to create semaphore array");
    exit (EXIT_FAILURE);
  } 

  semctl(semid, FULL_ID, SETVAL, 0);
  if (errno > 0) {
    perror("failed to set FULL semaphore");
    exit (EXIT_FAILURE);
  }

  semctl(semid, EMPTY_ID, SETVAL, QUEUE_SIZE);
  if (errno > 0) {
    perror("failed to set EMPTY sempahore");
    exit (EXIT_FAILURE);
  }

  semctl(semid, MUTEX_ID, SETVAL, 1);
  if (errno > 0) {
    perror("failed to create mutex");
  }

  return semid;
}