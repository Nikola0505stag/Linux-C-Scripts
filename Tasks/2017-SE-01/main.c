#include <fcntl.h> // O_WRONLY, O_RONLY, ...
#include <unistd.h> // read, write, lseek, ...
#include <err.h> // err, ...
#include <stdint.h> // uint8_t, ...
#include <sys/stat.h> // struct stat, fstat()
					
struct patch_file {
	uint16_t offset;
	uint8_t original_byte;
	uint8_t new_byte;
};

int main(int argc, char* argv[]) {

	if (argc != 4) {
		err(1, "Expected 3 arguments!");
	}

	int original_fd = open(argv[1], O_RDONLY);
	int new_fd = open(argv[2], O_RDONLY);
	int write_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (original_fd < 0) {
		err(2, "Can't open the original file!");
	}
	if (new_fd < 0) {
		err(3, "Can't open the new file!");
	}
	if (write_fd < 0) {
		err(4, "Can't open the output file!");
	}

	struct stat f1_stat;
	struct stat f2_stat;

	if (fstat(original_fd, &f1_stat) < 0) {
		err(5, "Error with fstat in the original file!");
	}

	if (fstat(new_fd, &f2_stat) < 0) {
		err(6, "Error with fstat in the new file!");
	}

	if (f1_stat.st_size != f2_stat.st_size) {
		err(7, "The files aren't the same size!");
	}

	uint16_t offset = 0;
	ssize_t exit_status1;
	ssize_t exit_status2;
	uint8_t first_file_letter;
	uint8_t second_file_letter;

	while ((exit_status1 = read(original_fd, &first_file_letter, sizeof(first_file_letter))) > 0 && (exit_status2 = read(new_fd, &second_file_letter, sizeof(second_file_letter))) > 0)     {
		if (first_file_letter != second_file_letter) {
			struct patch_file patch;
			patch.offset = offset;
			patch.original_byte = first_file_letter;
			patch.new_byte = second_file_letter;

			if (write(write_fd, &patch, sizeof(patch)) < 0) {
				err(8, "Error while writing in the patch file!");
			}
		}
		
		offset++;
	}

	close(original_fd);
	close(new_fd);
	close(write_fd);

	return 0;
}
