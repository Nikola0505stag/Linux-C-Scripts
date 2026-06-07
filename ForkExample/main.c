#include <stdio.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid < 0) {
        err(1, "could not fork");
    }

    if (pid > 0) {
        printf("Parent PID: %d\n", getpid());
        const char msg[] = "I am your father!\n";
        write(1, msg, strlen(msg));
    } else {
        printf("Child PID: %d\n", getpid());
        const char msg[] = "Noooooooo!\n";
        write(1, msg, strlen(msg));
    }

    return 0;
}
