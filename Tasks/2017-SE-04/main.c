#include <unistd.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char* argv[]) {

	if (argc == 1) {
		char byte;
		ssize_t read_result;
		while ((read_result = read(0, &byte, sizeof(byte))) > 0) {
			if (write(1, &byte, sizeof(byte)) < 0) {
				err(1, "Error writing the byte to stdout!");
			}				
		}

		if (read_result < 0) {
			err(2, "Error reading from stdint!");
		}
	} else {
		int count_arguments = argc - 1;
		char byte;
		ssize_t read_result;
		for (int i = 1; i < count_arguments; i++) {
			if (strcmp(argv[i], "-") == 0) {
				while ((read_result = read(0, &byte, sizeof(byte))) > 0) {
					if (write(1, &byte, sizeof(byte)) < 0) {
						err(8, "Error writing the byte to stdout!");
					}				
				}

				if (read_result < 0) {
					err(7, "Error reading from stdint!");
				}
		
			} else {
				int fd = open(argv[i], O_RDONLY);
				if (fd < 0) {
					err(3, "Error opening file for reading!");
				}

				while ((read_result = read(fd, &byte, sizeof(byte))) > 0) {
					if (write(1, &byte, sizeof(byte)) < 0) {
						err(4, "Error writing byte in stdout!");
					}
				}
				if (read_result < 0) {
					err(5, "Error reading from file!");
				}

				close(fd);
			}
		}	


	}


	return 0;
}
