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
#define SEM_NAME "/semaphore"

// Island structure to store in shared memory
typedef struct {
    int areas[ISLAND_AREAS];
    int treasure;
} island;

int pirates;
pid_t captain;
pid_t processes[1024];
island *shm;
sem_t *sem;

void cleanup() {
    // Wait for other processes
    while(wait(NULL) > 0);
    printf("Terminating captain process.\n");
    printf("Clearing shared memory and semaphore.\n");
    munmap(shm, sizeof(int) * pirates);
    shm_unlink(SHM_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);
    exit(0);
}

void sigint_handler() {
    cleanup();
}

void stop() {
    for (int i = 1; i <= pirates; ++i) {
        printf("Pirate %d returned to shore.\n", i);
	kill(processes[i-1], SIGUSR1);
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
    sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    int shmfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0644);
    ftruncate(shmfd, sizeof(island));
    shm = mmap(NULL, sizeof(island), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    close(shmfd);
    
    for (int i = 0; i < ISLAND_AREAS; ++i) {
        shm->areas[i] = -1;
    }
    
    srand(time(NULL));
    shm->treasure = rand() % 15;
    printf("Treasure is burried in area: %d\n", shm->treasure);

    pid_t pid;
    captain = getpid();
    
    // Building child processes that are simulating pirates
    for (int i = 1; i <= pirates; ++i) {
        pid = fork();
        if (pid < 0) {
            printf("Fork error!\n");
            exit(-1);
        } else if(pid == 0) {
            char str1[sizeof(int)], str2[sizeof(int)];
            sprintf(str1, "%d", i);
            sprintf(str2, "%d", captain);
            execl("./pirate", "./pirate", str1, str2, NULL);
            exit(0);
        } else {
            processes[i-1] = pid;
        }
    }

    for (int i = 0; i < pirates; ++i) {
        sem_post(sem);
    }

    // Wait for pirate processes
    pause();
}
