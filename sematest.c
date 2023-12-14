#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int initsem(key_t semkey)
{
    union semun semunarg;
    int status = 0, semid;

    // 세마포어 생성 또는 이미 존재하는 경우 기존 세마포어를 읽어옴
    semid = semget(semkey, 1, IPC_CREAT | IPC_EXCL | 0600);
    if (semid == -1)
    {
        if (errno == EEXIST)
            semid = semget(semkey, 1, 0);
    }
    else
    {
        // 세마포어 값을 1로 초기화 (잠금이 해제되어 있는 초기 상태)
        semunarg.val = 1;
        status = semctl(semid, 0, SETVAL, semunarg);
    }

    if (semid == -1 || status == -1)
    {
        perror("initsem");
        return (-1);
    }

    return semid;
}

int semlock(int semid)
{
    struct sembuf buf;

    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;

    if (semop(semid, &buf, 1) == -1)
    {
        perror("semlock failed");
        exit(1);
    }

    return 0;
}

int semunlock(int semid)
{
    struct sembuf buf;

    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;

    if (semop(semid, &buf, 1) == -1)
    {
        perror("semunlock failed");
        exit(1);
    }

    return 0;
}

void semhandle()
{
    int semid;
    pid_t pid = getpid();

    if ((semid = initsem(1)) < 0)
        exit(1);

    semlock(semid);
    printf("Lock : Process %d\n", (int)pid);
    printf("** Lock Mode : Critical Section\n");
    sleep(1);
    printf("Unlock : Process %d\n", (int)pid);
    semunlock(semid);

    exit(0);
}

int main()
{
    int a;
    for (a = 0; a < 3; a++)
        if (fork() == 0)
            semhandle();
}