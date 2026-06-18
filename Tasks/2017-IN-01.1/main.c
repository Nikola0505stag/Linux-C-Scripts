#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>

struct index_entry {
	uint16_t offset;
	uint8_t length;
	uint8_t reserved;
};

int main(int argc, char* argv[]) {
	if (argc != 5) {
		err(1, "Expected 4 arguments!");		
	}

	int fd1_dat = open(argv[1], O_RDONLY);
	int fd1_idx = open(argv[2], O_RDONLY);

	if (fd1_dat < 0) {
		err(2, "Error while opening first dat file!");
	}
	if (fd1_idx < 0) {
		err(3, "Error while opnening first idx file!");
	}

	int fd2_dat = open(argv[3], O_WRONLY);
	int fd2_idx = open(argv[4], O_WRONLY);

	if (fd2_dat < 0) {
		err(4, "Error while opening second dat file!");
	}
	if (fd2_idx < 0) {
		err(5, "Error while opening second idx file!");
	}

	struct stat st1_idx;
	if (fstat(fd1_idx, &st1_idx) < 0) {
		err(6, "Error getting stats for first idx file!");
	}
	if (st1_idx.st_size % sizeof(struct index_entry) != 0) {
		errx(7, "Error:  First idx file is corrupted!");
	}

	struct stat st1_dat;
	if (fstat(fd1_dat, &st1_dat) < 0) {
		err(8, "Error getting stats for first dat file!");
	}

	
	struct index_entry entry;
	ssize_t read_idx;
	uint16_t current_f2_dat_offset = 0;
	while ((read_idx = read(fd1_idx, &entry, sizeof(entry))) > 0) {
		
		if (entry.offset + entry.length > st1_dat.st_size) {
			errx(9, "Error: Index points outside of first dat file!");
		}

		if (entry.length == 0) {
			continue;
		}

		if (lseek(fd1_dat, entry.offset, SEEK_SET) < 0) {
			err(10, "Error seeking in first dat file!");
		}

		uint8_t first_char;

		if (read(fd1_dat, &first_char, sizeof(first_char)) < 0) {
			err(11, "Error reading first char from first dat file!");
		}

		
		if (first_char >= 0x41 && first_char <= 0x5A) {
			if (lseek(fd1_dat, -1, SEEK_CUR) < 0) {
				err(12, "Error seeking back in first dat file!");
			}

			uint8_t* string_buff = malloc(entry.length);
			if (string_buff == NULL) {
				err(13, "Memory allocation failed!");
			}

			if (read(fd1_dat, string_buff, entry.length) < 0) {
				err(14, "Error reading string from first dat file!");
			}

			if (write(fd2_dat, string_buff, entry.length) < 0) {
				err(15, "Error writing string to the second dat file!");
			}
			
			struct index_entry new_entry;

			new_entry.offset = current_f2_dat_offset;
			new_entry.length = entry.length;
			new_entry.reserved = 0x00;

			if (write(fd2_idx, &new_entry, sizeof(new_entry)) < 0) {
				err(16, "Error writing entry to the second idx file!");
			}

			current_f2_dat_offset += entry.length;

			free(string_buff);

		}

	}

	if (read_idx < 0) {
		err(17, "Error reading from first idx file!");
	}

	close(fd1_dat);
	close(fd1_idx);
	close(fd2_dat);
	close(fd2_idx);

	return 0;
}
