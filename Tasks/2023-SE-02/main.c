#include <stdio.h> // printf(), ...
#include <string.h> // strcmp(), ... 
#include <unistd.h> // read(), write(), ... 
#include <fcntl.h> // O_RDONLY, O_WRONLY, 
#include <sys/stat.h> // fstat(), ...
#include <err.h> // err(), errx(), ...
#include <stdint.h> // uint8_t, uint32_t, ... 
				

int main(int argc, char* argv[]) {
	if (argc != 3) {
		err(1, "Expected 2 arguments!");
	}

	char* target_word = argv[1];
	int fd = open(argv[2], O_RDONLY);
	if (fd < 0) {
		err(2, "Error opening dict!");
	}

	struct stat st;
	if (fstat(fd, &st) < 0) {
		err(3, "Error getting file stats!");
	}

	off_t file_size = st.st_size;

	if (file_size == 0) {
		errx(4, "Word not found (empty dictionary)!");
	}

	off_t low = 0;
	off_t high = file_size;

	while (low < high) {

		off_t mid = low + (high - low) / 2;
		if (lseek(fd, mid, SEEK_SET) < 0) {
			err(5, "Error seeking to mid!");
		}

		uint8_t c;
		int found_null = 0;

		while (lseek(fd, 0, SEEK_CUR) < high) {
			if (read(fd, &c, sizeof(c)) <= 0) {
				break;
			}
			if (c == '\0') {
				found_null = 1;
				break;
			}
		}

		if (!found_null) {
			high = mid;
			continue;
		}

		char current_word[64];
		int idx = 0;
		ssize_t read_res;

		while ((read_res = read(fd, &c, sizeof(c))) > 0 && c != '\n' && idx < 63) {
			current_word[idx++] = c;
		}
		current_word[idx] = '\0';

		int cmp = strcmp(current_word, target_word);

		if (cmp == 0) {
			ssize_t read_result;
			while ((read_result = read(fd, &c, 1)) > 0 && c != '\0') {
				if (write(1, &c, 1) < 0) {
					err(6, "Error writing to stdout!");
				}
			}

			if (read_result < 0) {
				err(7, "Error reading the description!");
			}

			if (write(1,"\n", 1) < 0) {
				err(8, "Error writing newline to stdout!");
			}

			close(fd);
			return 0;

		} else if (cmp > 0) {
			high = mid;
		} else {
			if (low == mid) {
				break;
			}
			low = mid;
		}
	}

	printf("Word not found!\n");
	close(fd);
	return 0;

}
