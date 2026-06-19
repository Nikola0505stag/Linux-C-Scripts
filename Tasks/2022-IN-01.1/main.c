#include <fcntl.h> // O_RDONLY, O_WRONLY, ...
#include <unistd.h> // read(), write(), ...
#include <stdint.h> // uint32_t, uint64_t, ...
#include <err.h> // err(), errx(), ...
				

struct header{
	uint16_t magic;
	uint16_t filetype;
	uint32_t count;
} __attribute__((packed));

int open_first_file(char* argv, struct header* header_list) {
	int fd_list = open(argv, O_RDONLY);
		if (fd_list < 0) {
			err(2, "Error occurred while opening the first file!");
		}
		
		if (read(fd_list, &header_list, sizeof(*header_list)) < 0) {
			err(3, "Error reading the header from the first file!");
		}

		if ((*header_list).magic != 0x5A4D) {
			errx(4, "Wrong magic value!");
		}

		if ((*header_list).filetype != 1) {
			errx(5, "Wrong file type!");
		}
	
		return fd_list;
}

int open_second_file (char* argv, struct header* header_data) {

	int fd_data = open(argv, O_RDONLY);
	if (fd_data < 0) {
		err(6, "Error occurred while opening the second file!");
	}
	
	if (read(fd_data, &header_data, sizeof(*header_data)) < 0) {
		err(7, "Error reading the header from the second file!");
	}

	if ((*header_data).magic != 0x5A4D) {
		err(8, "Wrong magic value!");
	}

	if ((*header_data).filetype != 2) {
		err(9, "Wrong file type!");
	}

	return fd_data;

}

int main(int argc, char* argv[]) {
	struct header header_list;
	struct header header_data;
	struct header header_out;

	
	if (argc != 4) {
		err(1, "Expected 3 arguments!");
	}

	int fd_list = open_first_file(argv[1], &header_list);
	int fd_data = open_second_file(argv[2], &header_data);
	
	int fd_out = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out < 0) {
		err(10, "Error occurred while opening the third file!");
	}
	
	if (read(fd_out, &header_out, sizeof(header_out)) < 0) {
		err(11, "Error reading the header from the third file!");
	}

	header_out.magic = 0x5A4D;
	header_out.filetype = 3;
	header_out.count = header_list.count;

	if (write(fd_out, &header_out, sizeof(header_out)) != sizeof(header_out)) {
		err(11, "Error writing header to the third file!");	
	}

	off_t total_size = sizeof(struct header) + ((off_t)header_out.count * sizeof(uint64_t));
	if (ftruncate(fd_out, total_size) < 0) {
		err(12, "Error resizing third file!");
	}

	uint16_t list_val;
	uint32_t data_val;

	for (uint32_t i = 0; i < header_list.count; i++) {
		if (read(fd_list, &list_val, sizeof(list_val)) != sizeof(list_val)) {
			err(13, "Error reading data element from first file!");
		}

		if (read(fd_data, &data_val, sizeof(data_val)) != sizeof(data_val)) {
			err(14, "Error reading data element from second file!");
		}

		if (data_val == 0) {
			continue;
		}

		uint64_t out_val = (uint64_t)data_val;

		off_t target_pos = sizeof(struct header) + ((off_t)list_val * sizeof(uint64_t));

		if (lseek(fd_out, target_pos, SEEK_SET) < 0) {
			err(15, "Error seeking to target position in third file!");
		}

		if (write(fd_out, &out_val, sizeof(out_val)) != sizeof(out_val)) {
			err(16, "Error writing data element to third file!");
		}
	}

	close(fd_list);
	close(fd_data);
	close(fd_out);

	return 0;
}
