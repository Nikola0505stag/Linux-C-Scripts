#include <stdio.h> // NULL
#include <unistd.h> // pipe(), fork(), read(), dup2(), close(), ...
#include <sys/wait.h> // wait(), ...
#include <err.h> // err(), errx(), ...


int main(int argc, char* argv[]) {
	
	if (argc != 2) {
		err(1, "Expected 1 argument!");
	}

	int pfd[2];
	if (pipe(pfd) < 0) {
		err(2, "Error creating pipe!");
	}

	pid_t pid = fork();
	if (pid < 0) {
		err(3, "Error forking!");
	}

	if (pid == 0) {
		// child:
		
		close(pfd[0]);
		if (dup2(pfd[1], 1) < 0) {
			err(4, "Error duplicating stdout in child!");
		}

		close(pfd[1]);

		execlp("cat", "cat", argv[1], (char*) NULL);
		err(5, "Exec cat failed!");
	} else {
		close(pfd[1]);

		if (dup2(pfd[0], 0) < 0) {
			err(6, "Error duplicating stdint in parent!");
		}
		close(pfd[0]);

		execlp("sort", "sort", (char*) NULL);
		err(7, "Exec sort failed!");
	}

	return 0;
}
