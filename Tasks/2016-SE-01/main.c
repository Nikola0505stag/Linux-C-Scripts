#include <fcntl.h> // O_RDWR
#include <unistd.h> // read, write, lseek
#include <err.h> // err and errx
#include <stdint.h> // uint32_t and uint8_t

int main(int argc, char* argv[]) {
	if (argc != 2) {
		err(1, "Need to start the program with bin file!");
	}

	int fd = open(argv[1], O_RDWR);

	if (fd < 0) {
		err(2, "Could not open fine %s!", argv[1]);
	}

	uint32_t counts[256] = {0};

	uint8_t byte;
	ssize_t read_size;

	while ((read_size = read(fd, &byte, sizeof(byte))) > 0) {
		counts[byte]++;
	}

	if (read_size < 0) {
		err(3, "Error reading from file!");
	}

	if (lseek(fd, 0, SEEK_SET) < 0) {
		err(4, "Error resetting file position!");
	}

	for (int i = 0; i < 256; i++) {
		while (counts[i] > 0) {
			uint8_t target_bite = i;

			if (write(fd, &target_bite, sizeof(target_bite)) < 0) {
				err(5, "Error writing to file!");
			}
			counts[i] --;
		}
	}


	close(fd);
	return 0;
}
