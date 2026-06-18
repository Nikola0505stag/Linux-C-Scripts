#include <err.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
	char command[100];

	while (1) {
		if (write(1, "my_shell> ", sizeof("my_shell> ")) < 0) {
			err(1, "Error writing the prompt!");
		}

		ssize_t bytes_read = read(0, command, 99);
		if (bytes_read <= 0) {
			err(2, "Error reading the command!");
		}

		command[bytes_read] = '\0';

		char* new_line = strchr(command, '\n');
		if (new_line != NULL) {
			*new_line = '\0';
		}

		if (command[0] == '\0') {
			continue;
		}

		if (strcmp(command, "exit") == 0) {
			break;
		}

		pid_t pid = fork();
		if (pid < 0) {
			err(3, "Fork failed!");
		}

		if (pid == 0) {
			char path[120];
			snprintf(path, sizeof(path), "/bin/%s", command);
			execl(path, command, (char*)NULL);

			if (write(2, "Command not found in /bin\n", 26) < 0) {
				err(4, "Error writing in default out!");
			}
			exit(127);
		} else {
			int status;
			wait(&status);
		}
	}

	if (write(1, "Goodbye!\n", 9) < 0) {
		err(5, "Error writing in default out!");
	}
	return 0;
}
