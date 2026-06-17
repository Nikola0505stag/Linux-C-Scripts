#include <stdio.h> // printf(), ...
#include <stdlib.h> // mkstemp(), ...
#include <unistd.h> // read(), write(), ...
#include <fcntl.h> // O_WRONLY, O_RDONLY, ...
#include <sys/stat.h> // fstat(), ...
#include <err.h> // err(), errx(), ...
#include <stdint.h> // uint32_t, uint8_t, ...

int index_found_in_temp_file(uint64_t index, int fd_temp) {
	uint64_t found_index;
	ssize_t read_result;

	if (lseek(fd_temp, 0, SEEK_SET) < 0) {
		err(8, "Error seeking to the beginning of the temp file!");
	}

	while ((read_result = read(fd_temp, &found_index, sizeof(found_index))) > 0) {
		if (found_index == index) {
			return 1;
		}
	}

	if (read_result < 0) {
		err(7, "Error reading indexes from temp file!");
	}

	return 0;
	
}

int main(int argc, char* argv[]) {
	
	if (argc != 2) {
		err(1, "Expected 1 argument!");
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		err(2, "Error occurred while opening file for reading!");
	}

	char temp_name[] = "/tmp/fileXXXXXX";
	int fd_temp = mkstemp(temp_name);
	if (fd_temp < 0) {
		err(3, "Error making the temp file!");
	}

	uint64_t current_idx = 0;
	while (1) {
		
		if (write(fd_temp, &current_idx, sizeof(current_idx)) != sizeof(current_idx)) {
			err(4, "Error writing to temp file!");
		}

		if (lseek(fd, current_idx * 512, SEEK_SET) < 0) {
			err(5, "Error seeking in first file!");
		}

		uint64_t next_idx;
		if (read(fd, &next_idx, sizeof(next_idx)) != sizeof(next_idx)) {
			err(6, "Error reading the next index from the first file!");
		}

		if (next_idx == 0) {
			break;
		}

		current_idx = next_idx;
	}

	close(fd);
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		err(9, "Error opening first file for read and write!");
	}


	char block[512];
	uint64_t i = 0;

	char zero_block[512];
	int j = 0;
	while (j < 512) {
		zero_block[j] = 0;
		j++;
	}

	while (read(fd, block, 512) == 512) {
		
		if (index_found_in_temp_file(i, fd_temp)) {
			continue;
		} else {
			if (lseek(fd, -512, SEEK_CUR) < 0) {
				err(5, "Error seeking back to block!");
			}

			if (write(fd, zero_block, 512) != 512) {
				err(12, "Error writing zeros!");
			}
		}

		i++;
	}

	return 0;
}
