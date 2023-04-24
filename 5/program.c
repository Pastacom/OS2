#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define ISLAND_AREAS 15
#define SHM_NAME "/shared_memory"

// Island structure to store in shared memory
typedef struct {
    int areas[ISLAND_AREAS];
    int treasure;
    sem_t sem;
} island;

int pirates;
pid_t parent;
pid_t childs[1024];
island *shm;

void cleanup() {
    // Wait for children
    while(wait(NULL) > 0);
    printf("Terminating parent.\n");
    printf("Clearing shared memory and semaphore.\n");
    munmap(shm, sizeof(int) * pirates);
    shm_unlink(SHM_NAME);
    exit(0);
}

void sigint_handler() {
    if (getpid() != parent) {
    	printf("Terminating child.\n");
    	exit(0);
    }
    cleanup();
}

void stop() {
    if (getpid() == parent) {
        for (int i = 1; i <= pirates; ++i) {
    	    printf("Pirate %d returned to shore.\n", i);
            kill(childs[i-1], SIGKILL);
        }
    }
    cleanup();
}

int main(int argc, char *argv[]) {
    // Check whether we have valid number of parameters
    if (argc != 2) {
        printf("Provide number of pirates.\n");
        exit(-1);
    }
    
    pirates = atoi(argv[1]);
    // Stopping program on getting CTRL+C, releasing memory and semaphore
    signal(SIGINT, sigint_handler);
    
    signal(SIGUSR1, stop);
    // Building semaphore and shared memory
    int shmfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0644);
    ftruncate(shmfd, sizeof(island));
    shm = mmap(NULL, sizeof(island), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    sem_init(&shm->sem, 1, 0);
    close(shmfd);
    
    for (int i = 0; i < ISLAND_AREAS; ++i) {
        shm->areas[i] = -1;
    }
    
    srand(time(NULL));
    shm->treasure = rand() % 15;
    printf("Treasure is burried in area: %d\n", shm->treasure);

    pid_t pid;
    parent = getpid();
    
    // Building child processes that are simulating pirates
    for (int i = 1; i <= pirates; ++i) {
        pid = fork();
        if (pid < 0) {
            printf("Fork error!\n");
            exit(-1);
        } else if(pid == 0) {
            for (int j = 0; j < ISLAND_AREAS; ++j) {
            	sem_wait(&shm->sem);
            	if (shm->areas[j] == -1) {
                    if (shm->treasure == j) {
                    	shm->areas[j] = 1;
                    	printf("Pirates %d: found treasure in area %d! Everyone, return to shore!\n", i, j);
                    	// Terminate all child processes
                    	kill(parent, SIGUSR1);
                    	sem_wait(&shm->sem);
                    } else {
                    	shm->areas[j] = 0;
                    	printf("Pirates %d: there is no chest in area %d! Keep searching!\n", i, j);
                    }
                    sleep(1 + rand() % 2);
            	}
                sem_post(&shm->sem);
            }
        } else {
            childs[i-1] = pid;
        }
    }

    for (int i = 0; i < pirates; ++i) {
        sem_post(&shm->sem);
    }

    // Wait for children
    while(wait(NULL) > 0);
    
    return 0;
}
