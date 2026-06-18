//find . -type f -exec ls -t {} + | head -n 1

#include <err.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		err(1, "Expected only one argument - directory!");
	}

	int pfd[2];
	if (pipe(pfd) < 0) {
		err(2, "Error creating the pipe!");
	}

	pid_t cpid = fork();
	if (cpid < 0) {
		err(3, "Fork failed!");
	}

	if (cpid == 0) {
		close(pfd[0]);

		if (dup2(pfd[1], 1) < 0) {
			err(4, "Error duplicating the pipe exit!");
		}
		close(pfd[1]);

		execlp("find", "find", argv[1], "-type", "f", "-exec", "ls", "-t", "{}", "+", (char*)NULL);
		err(5, "Error executing command find!");

	} else {
		close(pfd[1]);

		if (dup2(pfd[0], 0) < 0) {
			err(6, "Error duplicating the pipe in!");
		}
		close(pfd[0]);

		execlp("head", "head", "-n", "1", (char*)NULL);
		err(7, "Error executing command head!");
	}

	return 0;
}
