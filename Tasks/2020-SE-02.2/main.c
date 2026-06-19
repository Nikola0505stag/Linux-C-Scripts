#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main (int argc, char* argv[]) {
	if (argc != 3) {
		err(1, "Expected 2 arguments!");
	}

	int pfd[2];
	if (pipe(pfd) < 0) {
		err(2, "Pipe creation failed!");
	}

	pid_t pid = fork();
	if (pid < 0) {
		err(3, "Fork failed!");
	}

	if (pid == 0) {
		// child: exec the command
	
		close(pfd[0]); // won't read from the pipe
					   
		dup2(pfd[1], 1);
		close(pfd[1]);

		execlp("cat", "cat", argv[1], (char*)NULL);
		err(4, "Exec cat failed!");
	}

	// parent: transform the imput
		
	close(pfd[1]);
	int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (out_fd < 0) {
		err(5, "Error opening the second file!");
	}

	char c;
	ssize_t read_result;

	while ((read_result = read(pfd[0], &c, sizeof(c))) > 0) {
		if ((unsigned char)c == 0x55) {
			continue;
		}

		if ((unsigned char)c == 0x70) {
			char next_c;
			if (read(pfd[0], &next_c, sizeof(next_c)) > 0) {
				char decoded = next_c ^ 0x20;
				if (write(out_fd, &decoded, sizeof(decoded)) < 0) {
					err(6, "Error writing in the second file!");
				}
			}
		} else {
			if (write(out_fd, &c, sizeof(c)) < 0) {
				err(7, "Error writing in the second file!");
			}
		}
	}

	if (read_result < 0) {
		err(8, "Error reading from the pipe!");
	}
	
	close(pfd[0]);
	close(out_fd);

	wait(NULL);
	return 0;
}
