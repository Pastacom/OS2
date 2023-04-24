#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <wait.h>

#define ISLAND_AREAS 15

// Island structure to store in shared memory
typedef struct {
    int areas[ISLAND_AREAS];
    int treasure;
} island;

int pirate;
island *shm;
int shmid;
int semid;


void sigint_handler() {
    printf("Terminating pirate process.\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    // Check whether we have valid number of parameters
    if (argc != 3) {
        printf("Provide number of pirate and parent pid_t.\n");
        exit(-1);
    }
    
    pirate = atoi(argv[1]);
    pid_t parent = atoi(argv[2]);
    // Stopping program on getting CTRL+C, releasing memory and semaphore
    signal(SIGINT, sigint_handler);
    
    // Connecting to semaphore and shared memory
    key_t key = ftok("sem_file", 1);
    semid = semget(key, 1, 0644);
    
    key = ftok("shm_file", 1);
    shmid = shmget(key, sizeof(island), 0644);
    shm = (island*) shmat(shmid, NULL, 0);
    struct sembuf buf;
    
    srand(time(NULL));
    
    // Simulating pirates
    for (int j = 0; j < ISLAND_AREAS; ++j) {
        buf.sem_num = 0;
    	buf.sem_op = -1;
    	buf.sem_flg = 0;
    	semop(semid, &buf, 1);
    	if (shm->areas[j] == -1) {
            if (shm->treasure == j) {
            	shm->areas[j] = 1;
            	printf("Pirates %d: found treasure in area %d! Everyone, return to shore!\n", pirate, j);
            	// Terminate all pirate processes
            	kill(parent, SIGUSR1);
                buf.sem_num = 0;
	    	buf.sem_op = -1;
	    	buf.sem_flg = 0;
	    	semop(semid, &buf, 1);
            } else {
            	shm->areas[j] = 0;
            	printf("Pirates %d: there is no chest in area %d! Keep searching!\n", pirate, j);
            }
            sleep(1 + rand() % 2);
    	}
    	    buf.sem_num = 0;
	    buf.sem_op = 1;
	    buf.sem_flg = 0;
	    semop(semid, &buf, 1);
    }
    
    return 0;
}
