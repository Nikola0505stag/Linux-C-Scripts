#include <unistd.h> 
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	
	if (argc != 4) {
		err(1, "Expected 4 arguments!");
	}

	int fd_index =  open(argv[1], O_RDONLY);
	if (fd_index < 0) {
		err(2, "Error opening index file!");
	}

	int fd_data = open(argv[2], O_RDONLY);
	if (fd_data < 0) {
		err(3, "Error opening data file!");
	}

	char* word_to_find = argv[3];
	
	ssize_t read_result;
	uint8_t byte;
	int n = 0;
	
	while ((read_result = read(fd_index, &byte, sizeof(byte))) > 0) {
		uint8_t N = byte & 0x7F;
		uint8_t type = (byte >> 7) & 1;

		char* data = malloc(N + 1);
		if (data == NULL) {
			err(4, "Error allocating memory!");
		}
		data[N] = '\0';

		if (read(fd_index, &data, sizeof(data)) < 0) {
			err(5, "Error reading the data from the index file!");
		}

		if (strcmp(data, word_to_find) == 0) {
			if (lseek(fd_data, n * sizeof(uint32_t), SEEK_SET) < 0) {
				err(6, "Error seeking in data file!");
			}	

			if (type == 0) {
				uint32_t val;
				if (read(fd_data, &val, sizeof(val)) < 0) {
					errx(7, "Error reading the val from the data file!");
				}
				printf("%d\n",val);
			} else if (type == 1) {
				float val;
				if (read(fd_data, &val, sizeof(val)) < 0) {
					errx(8, "Error reading the val from the data file!");
				}
			}
		}	

		n++;
		free(data);
		close(fd_data);
		close(fd_index);
		return 0;
	}
	return 0;
}
