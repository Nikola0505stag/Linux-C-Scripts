#include <err.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	if (argc < 3) {
		err(1, "Expected at least 2 arguments!");
	}

	int number_of_files = argc - 2;

	int fdp[2];
	if (pipe(fdp) < 0) {
		err(2, "Error creating pipe!");
	}

	pid_t pid = fork();
	if (pid < 0) {
		err(3, "Fork failed!");
	}

	if (pid == 0) {
		close(fdp[0]);

		if (dup2(fdp[1], 1) < 0) {
			err(4, "Error duplicating!");
		}

		close(fdp[1]);

		execlp(argv[1], argv[1], (char*)NULL);
		err(5, "Error executing the program!");
	}

	close(fdp[1]);
	dup2(fdp[0], 0);
	close(fdp[0]);

	int file_idx = 0;
	char bytes[64];
	ssize_t read_result;

	while ((read_result = read(0, bytes, sizeof(bytes))) > 0) {
		int fd = open(argv[file_idx + 2], O_WRONLY | O_APPEND);
		if (fd < 0) {
			err(6, "Error opening file for writing!");
		}
		
		if (write(fd, bytes, read_result) < 0) {
			err(7, "Error writing the package to the file!");
		}
		
		close(fd);
			
		if (file_idx == number_of_files - 1) {
			file_idx = 0;
		} else {
			file_idx++;
		}
	}

	wait(NULL);
}
