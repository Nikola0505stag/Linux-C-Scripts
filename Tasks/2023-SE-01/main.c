#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	
	if (argc != 3) {
		err(1, "Expected 2 erguments!");
	}

	int fd1 = open(argv[1], O_RDONLY);
	if (fd1 < 0) {
		err(2, "Error opening the first file!");
	}

	int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd2 < 0) {
		err(3, "Error opening the second file!");
	}

	uint8_t first_byte;
	ssize_t read_result;

	while ((read_result = read(fd1, &first_byte, sizeof(first_byte))) > 0) {
		if (first_byte != 0x55) {
			continue;
		}
		
		uint8_t N;
		if (read(fd1, &N, sizeof(N)) < 0) {
			err(4, "Error reading N!");
		}
		if (N < 3) {
			lseek(fd1, -1, SEEK_CUR);
			continue;
		}

		int remaining_bytes = N - 2;
		uint8_t* bytes = malloc(remaining_bytes * sizeof(uint8_t));
		if (bytes == NULL) {
			err(5, "Memory allocation failed!");
		}
		
		ssize_t read_bytes_N;
		if ((read_bytes_N = read(fd1, bytes, remaining_bytes)) != remaining_bytes) {
			free(bytes);
			lseek(fd1, - (1 + read_bytes_N), SEEK_CUR);
			continue;
		}

		uint8_t result = 0x55 ^ N;
		for (int i = 0; i < remaining_bytes - 1; i++) {
			result = result ^ bytes[i];
		}
		uint8_t file_cheksum = bytes[remaining_bytes - 1];
	
		if (result == file_cheksum) {
			uint8_t byte1 = 0x55;
			if (write(fd2, &byte1, sizeof(byte1)) < 0) {
				err(6, "Error writing in the second file!");	
			}
			if (write(fd2, &N, sizeof(N)) < 0) {
				err(7, "Error writing in the second file!");	
			}
			if (write(fd2, bytes, remaining_bytes) < 0) {
				err(8, "Error writing in the second file!");
			}

			free(bytes);
		} else {
			free(bytes);
			lseek(fd1, -((int)N + 1),SEEK_CUR);
			continue;
		}
	}

	if (read_result < 0) {
		err(9, "Error reading from the first file!");
	}

	close(fd1);
	close(fd2);

}
