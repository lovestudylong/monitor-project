#ifndef __SEM_H__
#define __SEM_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int sem_p(int semid, int num)
{
    struct sembuf mybuf;
    mybuf.sem_num = num;
	mybuf.sem_op = -1;
	mybuf.sem_flg = SEM_UNDO;
    if (semop(semid, &mybuf, 1) < 0) {
        perror("semop");
		exit(-1);
    }
    return 0;
}

int sem_v(int semid, int num)
{
    struct sembuf mybuf;
	mybuf.sem_num = num;
	mybuf.sem_op = 1;
	mybuf.sem_flg = SEM_UNDO;
	if (semop(semid, &mybuf, 1) < 0) {
		perror("semop");
		exit(-1);
	}
	return 0;
}

#endif