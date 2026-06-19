#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdint.h>
#include <stdio.h>

struct triplet {
	char filename[8];
	uint32_t offset;
	uint32_t length;
} __attribute__((packed));

int main(int argc, char* argv[]) {
	if (argc != 2) {
		errx(1, "Usage: %s <binary_file>", argv[0]);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		err(2, "Error opening input file");
	}

	int pfd[2];
	if (pipe(pfd) < 0) {
		err(3, "Pipe failed");
	}

	struct triplet t;
	int children_count = 0;

	while (read(fd, &t, sizeof(t)) == sizeof(t)) {
		
		pid_t pid = fork();
		if (pid < 0) {
			err(4, "Fork failed");
		}

		if (pid == 0) {
			close(pfd[0]);

			int child_fd = open(t.filename, O_RDONLY);
			if (child_fd < 0) {
				warn("Child failed to open file %s", t.filename);
				uint16_t zero = 0;
				if (write(pfd[1], &zero, sizeof(zero)) < 0) {
					err(9, "Error writing!");
				}
				_exit(1);
			}

			if (lseek(child_fd, t.offset * sizeof(uint16_t), SEEK_SET) < 0) {
				warn("Lseek failed in child for %s", t.filename);
				uint16_t zero = 0;
				if (write(pfd[1], &zero, sizeof(zero)) < 0) {
					err(10, "Error writing!");
				}
				_exit(1);
			}

			uint16_t current_num;
			uint16_t child_xor_result = 0;
			uint32_t items_read = 0;

			while (items_read < t.length && read(child_fd, &current_num, sizeof(uint16_t)) == sizeof(uint16_t)) {
				child_xor_result ^= current_num;
				items_read++;
			}

			close(child_fd);

			if (write(pfd[1], &child_xor_result, sizeof(child_xor_result)) < 0) {
				err(11, "Error writing!");
			}
			close(pfd[1]);
			
			_exit(0);
		}

		children_count++;
	}

	close(fd);
	close(pfd[1]);

	uint16_t final_xor_result = 0;
	uint16_t received_res;

	while (read(pfd[0], &received_res, sizeof(uint16_t)) == sizeof(uint16_t)) {
		final_xor_result ^= received_res;
	}

	close(pfd[0]);

	for (int i = 0; i < children_count; i++) {
		wait(NULL);
	}

	printf("result:\n%04X\n", final_xor_result);

	return 0;
}
