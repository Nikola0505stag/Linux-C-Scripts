#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
	if (argc < 3 || argc > 4) {
		err(1, "Expected 2 or 3 file arguments!");
	}

	int fd1 = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_RDONLY);

	if (fd1 < 0) {
		err(2, "Error with openning the first file!");
	}

	if (fd2 < 0) {
		err(3, "Error with openning the second file!");
	}

	ssize_t count_numbers = 0;
	uint32_t buffer;
	int exit_code;

	while ( (exit_code = read(fd2, &buffer, sizeof(buffer))) > 0 ) {
		count_numbers++;
	}

	if (exit_code < 0) {
		err(4, "Error while reading from second file!");
	}

	if (lseek(fd2, 0, SEEK_SET) < 0) {
		err(5, "Error returing to the begging of the second file!");
	}

	uint32_t nums[count_numbers];
	ssize_t index = 0;

	while ( (exit_code = read(fd2, &buffer, sizeof(buffer))) > 0 ) {
		nums[index] = buffer;
		index++;
	}

	if (exit_code < 0) {
		err(6, "Error while reading from second file!");
	}

	close(fd2);

	const char* output_filename;
	if (argc == 4) {
		output_filename = argv[4];
	} else {
		output_filename = "f3.bin";
	}

	int fd3 = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd3 < 0) {
		err(7, "Error with opening the third file!");
	}

	uint32_t buffer2;
	ssize_t exit_code2 = 0;

	while ( (exit_code = read(fd1, &buffer, sizeof(buffer))) > 0 && (exit_code2 = read(fd1, &buffer2, sizeof(buffer2))) > 0) {
			
		for (uint32_t i = 0; i < buffer2; i++) {
			uint32_t current_index = buffer + i;
			
			uint32_t buff_to_write = nums[current_index];

			if (write(fd3, &buff_to_write, sizeof(buff_to_write)) < 0) {
				err(8, "Error while writing in third file!");
			}
		}

	}

	if (exit_code < 0 || exit_code2 < 0) {
		err(9, "Error while reading from first file!");
	}

	close(fd1);
	close(fd3);

}
