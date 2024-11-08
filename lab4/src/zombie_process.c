#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork(); // Создаем новый процесс

    if (pid < 0) {
        perror("Fork failed");
        exit(1); // Ошибка при создании процесса
    }

    if (pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс с PID %d\n", getpid());
        sleep(2); // Имитация работы дочернего процесса
        printf("Дочерний процесс завершен\n");
        exit(0); // Завершение дочернего процесса
    } else {
        // Родительский процесс
        printf("Родительский процесс с PID %d создал дочерний процесс с PID %d\n", getpid(), pid);
        
        // Не вызываем wait(), чтобы создать зомби-процесс
        sleep(10); // Ожидание перед завершением родительского процесса
        printf("Родительский процесс завершен\n");
    }

    return 0;
}