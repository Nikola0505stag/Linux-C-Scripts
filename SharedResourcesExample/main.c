#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(void) {
	int fd = open("./test.txt", O_WRONLY|O_CREAT|O_TRUNC, 066);

	if (fd < 0) {
		err(1, "could not open file");
	}

	pid_t pid = fork();
	if (pid < 0) {
		err(1, "could not fork");
	}

	for (int i = 0; i < 1000; i++) {
		if (pid == 0) {
			write(fd, "foo\n", 4);
		} else {
			write(fd, "bar\n", 4);
		}
	}

	close(fd);
}
