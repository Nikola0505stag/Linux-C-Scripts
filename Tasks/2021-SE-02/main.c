#include <unistd.h> // read(), write(), lseek(), ...
#include <fcntl.h> // O_RDONLY, O_WRONLY
#include <err.h> // err(), errx(), ...
#include <stdint.h> // uint32_t, ...


int main(int argc, char* argv[]) {

	if (argc != 3) {
		err(1, "Expected 2 arguments!");
	}

	int fd1 = open(argv[1], O_RDONLY);
	if (fd1 < 0) {
		err(2, "Error opening the first file!");
	}

	int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd2 < 0) {
		err(3, "Error opening the second file!");
	}

	uint16_t bytes;
	ssize_t read_result;

	while ((read_result = read(fd1, &bytes, sizeof(bytes))) > 0) {

		if (read_result != sizeof(bytes)) {
			errx(4, "Corrupted file!");
		}

		uint8_t to_write = 0x00;

		if ((bytes & 0xC000) == 0x8000) {
			to_write |= 0x80;
		} else if ((bytes & 0xC000) != 0x4000) {
			errx(5, "Invalid Manchester code!");
		}

		if ((bytes & 0x3000) == 0x2000) {
			to_write |= 0x40;
		} else if ((bytes & 0x3000) != 0x1000) {
			errx(6, "Invalid Manchester code!");
		}

		if ((bytes & 0x0C00) == 0x0800) {
			to_write |= 0x20;
		} else if ((bytes & 0x0C00) != 0x0400) {
			errx(7, "Invalid Manchester code!");
		}

		if ((bytes & 0x0300) == 0x0200) {
			to_write |= 0x10;
		} else if ((bytes & 0x0300) != 0x0100) {
			errx(8, "Invalid Manchester code!");
		}

		if ((bytes & 0x00C0) == 0x0080) {
			to_write |= 0x08;
		} else if ((bytes & 0x00C0) != 0x0040) {
			errx(9, "Invalid Manchester code!");
		}

		if ((bytes & 0x0030) == 0x0020) {
			to_write |= 0x04;
		} else if ((bytes & 0x0030) != 0x0010) {
			errx(10, "Invalid Manchester code!");
		}

		if ((bytes & 0x000C) == 0x0008) {
			to_write |= 0x02;
		} else if ((bytes & 0x000C) != 0x0004) {
			errx(11, "Invalid Manchester code!");
		}

		if ((bytes & 0x0003) == 0x0002) {
			to_write |= 0x01;
		} else if ((bytes & 0x0003) != 0x0001) {
			errx(12, "Invalid Manchester code!");
		}

		if (write(fd2, &to_write, sizeof(to_write)) != sizeof(to_write)) {
			err(13, "Error writing to the second file!");
		}
	}

	if (read_result < 0) {
		err(14, "Error reading from first file!");
	}
}
