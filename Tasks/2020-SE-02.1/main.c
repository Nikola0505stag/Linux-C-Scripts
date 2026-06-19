#include <unistd.h> // read(), write(), close(), ...
#include <fcntl.h> // O_WRONLY, O_RDONLY, ...
#include <stdlib.h>
#include <stdint.h> // uint32_t, ...
#include <err.h> // err(), errx(), ...


int main(int argc, char* argv[]) {
	if (argc != 4) {
		err(1, "Expected 3 arguments!");
	}

	int fd1 = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_RDONLY);
	int fd3 = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, 0644);

	if (fd1 < 0) {
		err(2, "Error occurred while opening the first file!");
	}

	if (fd2 < 0) {
		err(3, "Error occurred while opening the second file!");
	}

	if (fd3 < 0) {
		err(4, "Error occurred while opening the third file!");
	}

	uint8_t bit;
	uint16_t num;
	ssize_t result1;
	uint8_t nums[8];

	while ((result1 = read(fd1, &bit, sizeof(bit))) > 0) {
		if (bit & 0x80) {
			nums[0] = 1;
		} else {
			nums[0] = 0;
		}

		if (bit & 0x40) {
			nums[1] = 1;
		} else {
			nums[1] = 0;
		}

		if (bit & 0x20) {
			nums[2] = 1;
		} else {
			nums[2] = 0;
		}

		if (bit & 0x10) {
			nums[3] = 1;
		} else {
			nums[3] = 0;
		}

		if (bit & 0x08) {
			nums[4] = 1;
		} else {
			nums[4] = 0;
		}

		if (bit & 0x04) {
			nums[5] = 1;
		} else {
			nums[5] = 0;
		}

		if (bit & 0x02) {
			nums[6] = 1;
		} else {
			nums[6] = 0;
		}

		if (bit & 0x01) {
			nums[7] = 1;
		} else {
			nums[7] = 0;
		}

		for (int i = 0; i < 8; i++) {
			if (read(fd2, &num, sizeof(num)) < 0) {
				err(5, "Error reading from the second file!");
			}	
			if (nums[i]) {
				if (write(fd3, &num, sizeof(num)) < 0) {
					err(6, "Error writing in the third file!");
				}
			}
		}
	}

	if (result1 < 0) {
		err(7, "Error reading from the first file!");
	}

	close(fd1);
	close(fd2);
	close(fd3);

	return 0;
}
