#include <fcntl.h> // O_WRONLY, O_RDONLY, ...
#include <err.h> // err(), errx(), ...
#include <unistd.h> // read(), write(), close(), ... 
#include <stdint.h> // uint32_t, ...

int main(int argc, char* argv[]) {
	if (argc != 3) {
		err(1, "Expected 2 arguments!");
	}

	int fd1 = open(argv[1], O_RDONLY);
	if (fd1 < 0) {
		err(2, "Error occurred while opening the first file!");
	}

	int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd2 < 0) {
		err(3, "Error occurred while opening the second file!");
	}


	uint8_t byte;
	ssize_t read_result;
	uint8_t nums[16];

	while ((read_result = read(fd1, &byte, sizeof(byte))) > 0) {

		if (byte & 0x80) { nums[0] = 1;  nums[1] = 0;  } else { nums[0] = 0;  nums[1] = 1;  }
		if (byte & 0x40) { nums[2] = 1;  nums[3] = 0;  } else { nums[2] = 0;  nums[3] = 1;  }
		if (byte & 0x20) { nums[4] = 1;  nums[5] = 0;  } else { nums[4] = 0;  nums[5] = 1;  }
		if (byte & 0x10) { nums[6] = 1;  nums[7] = 0;  } else { nums[6] = 0;  nums[7] = 1;  }
		if (byte & 0x08) { nums[8] = 1;  nums[9] = 0;  } else { nums[8] = 0;  nums[9] = 1;  }
		if (byte & 0x04) { nums[10] = 1; nums[11] = 0; } else { nums[10] = 0; nums[11] = 1; }
		if (byte & 0x02) { nums[12] = 1; nums[13] = 0; } else { nums[12] = 0; nums[13] = 1; }
		if (byte & 0x01) { nums[14] = 1; nums[15] = 0; } else { nums[14] = 0; nums[15] = 1; }

		uint8_t out_byte1 = 0;
		for (int i = 0; i < 8; i++) {
			out_byte1 |= (nums[i] << (7 - i));
		}

		uint8_t out_byte2 = 0;
		for (int i = 0; i < 8; i++) {
			out_byte2 |= (nums[i + 8] << (7 - i));
		}

		if (write(fd2, &out_byte1, sizeof(out_byte1)) != sizeof(out_byte1)) {
			err(4, "Error writing first byte to output file");
		}
		if (write(fd2, &out_byte2, sizeof(out_byte2)) != sizeof(out_byte2)) {
			err(5, "Error writing second byte to output file");
		}
	}

	if (read_result < 0) {
		err(6, "Error reading from input file");
	}

	close(fd1);
	close(fd2);
}
