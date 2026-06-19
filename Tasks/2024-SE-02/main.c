#include <err.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 11) {
		errx(1, "Expected between 1 and 10 arguments!");
	}
	
	int number_of_programs = argc - 1;
	pid_t pids[number_of_programs];
	int ran_good_counter = 0;

	for (int i = 0; i < number_of_programs; i++) {
		pid_t pid = fork();
		if (pid < 0) {
			err(2, "Fork failed!");
		}

		if (pid == 0) {
			execlp(argv[i + 1], argv[i + 1], (char*)NULL);
			err(3, "Exec failed!");
		}

		pids[i] = pid;
	}

	int status;
	pid_t dead_pid;

	while (ran_good_counter < number_of_programs && (dead_pid = waitpid(-1, &status, 0)) > 0) {
		
		int idx = -1;
		for (int i = 0; i < number_of_programs; i++) {
			if (pids[i] == dead_pid) {
				idx = i;
				break;
			}
		}

		if (WIFSIGNALED(status)) {
			for (int i = 0; i < number_of_programs; i++) {
				if (pids[i] != dead_pid && pids[i] > 0) {
					kill(pids[i], SIGTERM);
					waitpid(pids[i], NULL, 0);
				}
			}
			exit(idx + 1);
		}

		if (WIFEXITED(status)) {
			int exit_code = WEXITSTATUS(status);

			if (exit_code == 0) {
				ran_good_counter++;
				pids[idx] = 0;
			} else {
				pid_t new_pid = fork();
				if (new_pid < 0) {
					err(4, "Fork failed during restart!");
				}

				if (new_pid == 0) {
					execlp(argv[idx + 1], argv[idx + 1], (char*)NULL);
					err(5, "Exec failed during restart!");
				}

				pids[idx] = new_pid;
			}
		}
	}

	return 0;
}
