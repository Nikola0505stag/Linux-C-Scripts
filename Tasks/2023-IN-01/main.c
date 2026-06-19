#include <fcntl.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

const char* words[] = {"tic ", "tac ", "toe\n"};

void pipeRead(int from, int to, int limit) {
	int curr;
	
	while (read(from, &curr, sizeof(curr)) == sizeof(curr)) {
		if (curr < limit) {
			curr++;
			
			if (write(to, &curr, sizeof(curr)) != sizeof(curr)) {
				err(1, "cannot write token!");
			}
			
			if (write(1, words[(curr - 1) % 3], 4) != 4) {
				err(1, "cannot write word!");
			}
		} else {
			write(to, &curr, sizeof(curr));
			break;
		}
	}
	
	close(from);
	close(to);
	exit(0);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		errx(1, "expected 2 args!");
	}

	int NC = strtol(argv[1], NULL, 10);
	int WC = strtol(argv[2], NULL, 10);
	int counter = 0;

	if (NC < 1 || NC > 7 || WC < 1 || WC > 35) {
		errx(1, "invalid count!");
	}

	int pipesfd[16][2];

	for (int i = 0; i <= NC; i++) {
		if (pipe(pipesfd[i]) < 0) {
			err(1, "cannot pipe!");
		}
	}

	for (int i = 0; i < NC; i++) {
		int pid = fork();

		if (pid < 0) {
			err(1, "cannot fork!");
		}

		if (pid == 0) {
			for (int j = 0; j <= NC; j++) {
				if (j == i) {
					close(pipesfd[j][1]);
				} else if (j == i + 1) {
					close(pipesfd[j][0]);
				} else {
					close(pipesfd[j][0]);
					close(pipesfd[j][1]);
				}
			}
			
			pipeRead(pipesfd[i][0], pipesfd[i + 1][1], WC);
		}
	}

	for (int i = 0; i <= NC; i++) {
		if (i == NC) {
			close(pipesfd[i][1]);
		} else if (i == 0) {
			close(pipesfd[0][0]);
		} else {
			close(pipesfd[i][0]);
			close(pipesfd[i][1]);
		}
	}

	if (write(pipesfd[0][1], &counter, sizeof(int)) != sizeof(int)) {
		err(1, "cannot write initial token!");
	}
	
	close(pipesfd[0][1]);

	int curr;
	while (read(pipesfd[NC][0], &curr, sizeof(curr)) == sizeof(curr)) {
		if (curr < WC) {
			curr++;
			
			if (write(1, words[(curr - 1) % 3], 4) != 4) {
				err(1, "cannot write!");
			}
			
		} else {
			break;
		}
	}

	close(pipesfd[NC][0]);

	for (int i = 0; i < NC; i++) {
		wait(NULL);
	}

	return 0;
}
