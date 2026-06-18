#include <unistd.h> 
#include <err.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	const char* cmd = "echo";
	if (argc > 1) {
		cmd = argv[1];
		if (strlen(cmd) > 4) {
			errx(1, "Command name too long! Max 4 characters.");
		}
	}

	char arg1[5];
	char arg2[5];

	while (1) {
		int args_count = 0;
		
		int idx1 = 0;
		char c;
		ssize_t b_read;

		while ((b_read = read(0, &c, 1)) > 0) {
			if (c == 0x20 || c == 0x0A) { 
				if (idx1 == 0) {
					continue;  
				}
				break;                    
			}
			if (idx1 >= 4) {
				errx(2, "Argument too long! Max 4 characters.");
			}
			arg1[idx1++] = c;
		}
		if (b_read < 0) err(3, "Error reading from stdin");
		if (b_read == 0 && idx1 == 0) {
			break;
		}
		arg1[idx1] = '\0';
		args_count++;

		int idx2 = 0;
		while ((b_read = read(0, &c, 1)) > 0) {
			if (c == 0x20 || c == 0x0A) {
				if (idx2 == 0) continue;
				break;
			}
			if (idx2 >= 4) {
				errx(2, "Argument too long! Max 4 characters.");
			}
			arg2[idx2++] = c;
		}
		if (b_read < 0) err(3, "Error reading from stdin");
		if (idx2 > 0) {
			arg2[idx2] = '\0';
			args_count++;
		}

		pid_t pid = fork();
		if (pid < 0) {
			err(4, "Fork failed!");
		}

		if (pid == 0) {
			if (args_count == 1) {
				execlp(cmd, cmd, arg1, (char*)NULL);
			} else if (args_count == 2) {
				execlp(cmd, cmd, arg1, arg2, (char*)NULL);
			}
			err(5, "Exec failed for command: %s", cmd);
		} else {
			int status;
			wait(&status);
		}
	}

	return 0;
}
