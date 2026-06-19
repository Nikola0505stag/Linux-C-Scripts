#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		err(1, "Expected 1 argument!");
	}

	pid_t pid = fork();
	if (pid < 0) {
		err(2, "Fork failed!");
	}

	if (pid == 0) {
		int fifo_fd = open("my_fifo", O_RDONLY);
		if (fifo_fd < 0) {
			err(3, "Child failed to open FIFO for reading!");
		}

		dup2(fifo_fd, 0);
		close(fifo_fd);

		execl(argv[1], argv[1], (char*)NULL);
		err(4, "Exec command failed in bar!");
	}

	wait(NULL);
	unlink("my_fifo");
	return 0;
}
