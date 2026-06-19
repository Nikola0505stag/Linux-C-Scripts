#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		err(1, "Expected 1 argument!");
	}

	mkfifo("my_fifo", 0666);

	pid_t pid = fork();
	if (pid < 0) {
		err(2, "Fork failed in foo!");
	}

	if (pid == 0) {
		int fifo_fd = open("my_fifo", O_RDONLY);
		if (fifo_fd < 0) {
			err(3, "Child failed to open FIFO for writing!");
		}
		dup2(fifo_fd, 1);
		close(fifo_fd);

		execlp("cat", "cat", argv[1], (char*)NULL);
		err(4, "Exec cat failed!");
	}

	wait(NULL);
	return 0;
}
