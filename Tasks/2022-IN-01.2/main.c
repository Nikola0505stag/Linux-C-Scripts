#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		err(1, "Expected 2 arguments!");
	}

	int N = strtol(argv[1], NULL, 10);
	int D = strtol(argv[2], NULL, 10);

	int parent_to_child[2];
	int child_to_parent[2];
	
	if (pipe(parent_to_child) < 0) {
		err(2, "Error creating the first pipe!");
	}

	if (pipe(child_to_parent) < 0) {
		err(3, "Error creating the second pipe!");
	}

	int pid = fork();
	if (pid < 0) {
		err(4, "Fork failed!");
	}

	if (pid == 0) {
		close(parent_to_child[1]);
		close(child_to_parent[0]);

		char token;

		for (int i = 0; i < N; i++) {
			if (read(parent_to_child[0], &token, 1) < 0) {
				err(5, "Error reading the token!");
			}

			if (write(1, "DONG\n", 5) < 0) {
				err(6, "Error writing DONG!");
			}

			if (write(child_to_parent[1], "X", 1) < 0) {
				err(7, "Error writing the token!");
			}
		}

		close(parent_to_child[0]);
		close(child_to_parent[1]);
		exit(0);
	}

	close(parent_to_child[0]);
	close(child_to_parent[1]);

	char token;

	for (int i = 0; i < N; i++) {
		if (write(1, "DING ", 5) < 0) {
			err(8, "Error writing DING!");
		}

		if (write(parent_to_child[1], "X", 1) < 0) {
			err(9, "Error writing the token!");
		}

		if (read(child_to_parent[0], &token, 1) < 0) {
			err(10, "Error reading the token!");
		}

		sleep(D);
	}

	close(parent_to_child[1]);
	close(child_to_parent[0]);

	wait(NULL);
	return 0;
}
