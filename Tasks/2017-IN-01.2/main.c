#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

int main(void) {
	int pfd1[2]; // between cut and sort
	int pfd2[2]; // between sort and uniq

	if (pipe(pfd1) < 0) {
		err(1, "Error creating the first pipe!");
	}

	pid_t pid1 = fork();
	if (pid1 < 0) {
		err(2, "Fork 1 failed!");
	}

	if (pid1 == 0) {
		close(pfd1[0]);

		dup2(pfd1[1], 1);
		close(pfd1[1]);

		execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char*)NULL);
		err(3, "Exec cut failed");
	}

	if (pipe(pfd2) < 0) {
		err(4, "Error creating the second pipe!");
	}

	pid_t pid2 = fork();
	if (pid2 < 0) {
		err(5, "Fork 2 failed!");
	}

	if (pid2 == 0) {
		close(pfd1[1]);
		dup2(pfd1[0], 0);
		close(pfd1[0]);

		close(pfd2[0]);
		dup2(pfd2[1], 1);
		close(pfd2[1]);

		execlp("sort", "sort", (char*)NULL);
		err(6, "Exec sort failed!");
	}

	close(pfd1[0]);
	close(pfd1[1]);

	close(pfd2[1]);

	dup2(pfd2[0], 0);
	close(pfd2[0]);

	execlp("uniq", "uniq", "-c", (char*)NULL);
	err(7, "Exec uniq failed!");

	return 0;
}
