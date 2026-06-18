#include <err.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	if (argc < 3) {
		err(1, "Expected at least 2 arguments!");
	}

	int threshold = argv[1][0] - '0';
	if (threshold < 1 || threshold > 9) {
		err(2, "Threshold must be single digit between 1 and 9!");
	}

	int log_fd = open("run.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (log_fd < 0) {
		err(3, "Error opening/creating the log file!");
	}
	
	int consecutive_failures = 0;

	while (1) {
		time_t start_time = time(NULL);

		pid_t cpid = fork();
		if (cpid < 0) {
			err(4, "Fork failed!");
		}

		if (cpid == 0) {
			execvp(argv[2], &argv[2]);
			err(6, "Execvp failed!");
		}

		int status;
		wait(&status);
		time_t end_time = time(NULL);

		long duration = end_time - start_time;
		int exit_code = 0;

		if (WIFEXITED(status)) {
			exit_code = WEXITSTATUS(status);
		} else if(WIFSIGNALED(status)) {
			exit_code = 129;
		}

		char log_buffer[150];
		int bytes_to_write = snprintf(log_buffer, sizeof(log_buffer), "%ld %ld %d\n", 
										(long)start_time, (long)end_time, exit_code);

		if (write(log_fd, log_buffer, bytes_to_write) < 0) {
			err(7, "Error writing to log file!");
		}

		if (exit_code != 0 && duration < threshold) {
			consecutive_failures++;
		} else {
			consecutive_failures = 0;
		}

		if (consecutive_failures == 2) {
			break;
		}
	}
	close(log_fd);
	return 0;
}
