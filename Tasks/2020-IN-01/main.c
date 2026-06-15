#include <unistd.h> // read(), write(), lseek(), close(), ...
#include <fcntl.h> // O_WRONLY, ...
#include <stdlib.h> // malloc(), free(), ...
#include <stdint.h> // uint32_t, uint8_t, ...
#include <string.h> // strcmp(), ...
#include <err.h> // err(), errx(), ...

struct header {
	uint32_t magic;
	uint8_t header_version;
	uint8_t data_version;
	uint16_t count;
	uint32_t reserved_1;
	uint32_t reserved_2;
} __attribute__((packed));

struct version_0 {
	uint16_t offset;
	uint8_t original_byte;
	uint8_t new_byte;
} __attribute__((packed));

struct version_1 {
	uint32_t offset;
	uint16_t original_byte;
	uint16_t new_byte;
} __attribute__((packed));

void transfer_file(void* byte, size_t size, int fd1, int fd2) {
	ssize_t read_result;
	while ((read_result = read(fd1, &byte, size)) > 0) {
		if (write(fd2, &byte, sizeof(byte)) < 0) {
			err(7, "Error occurred while transfering the data to the second file!");
		}
	}

	if (read_result < 0) {
		err(8, "Error occurred while reading the data from the first file!");
	}

	close(fd1);
	close(fd2);
}

void patch_version_0(int fd_patch, int fd2, uint16_t count) {
	struct version_0 vers;
	uint8_t acctual_byte;
	for (uint16_t i = 0; i < count; i++) {
		if (read(fd_patch, &vers, sizeof(vers)) < 0) {
			err(9, "Error reading the data from the patch file!");
		}
		if (lseek(fd2, vers.offset * sizeof(uint8_t), SEEK_SET) < 0) {
			err(10, "Error seeking to the position!");
		}

		if (read(fd2, &acctual_byte, sizeof(acctual_byte)) !=  sizeof(acctual_byte)) {
			err(11, "Error reading the byte!");
		}

		if (acctual_byte != vers.original_byte) {
			err(12, "Wrong byte!");
		}

		if (lseek(fd2, -sizeof(uint8_t), SEEK_CUR) < 0) {
			err(13, "Error seeking back!");
		}

		if (write(fd2, &vers.new_byte, sizeof(vers.new_byte)) < 0) {
			err(14, "Error writing the new byte!");
		}
	}
}

void patch_version_1(int fd_patch, int fd2, uint16_t count) {
	struct version_1 vers;
	uint16_t acctual_byte;
	for (uint32_t i = 0; i < count; i++) {
		if (read(fd_patch, &vers, sizeof(vers)) < 0) {
			err(15, "Error reading the data from the patch file!");
		}
		if (lseek(fd2, vers.offset * sizeof(uint16_t), SEEK_SET) < 0) {
			err(16, "Error seeking to the position!");
		}

		if (read(fd2, &acctual_byte, sizeof(acctual_byte)) !=  sizeof(acctual_byte)) {
			err(17, "Error reading the byte!");
		}

		if (acctual_byte != vers.original_byte) {
			err(18, "Wrong byte!");
		}

		if (lseek(fd2, -sizeof(uint16_t), SEEK_CUR) < 0) {
			err(19, "Error seeking back!");
		}

		if (write(fd2, &vers.new_byte, sizeof(vers.new_byte)) < 0) {
			err(20, "Error writing the new byte!");
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 4) {
		err(1, "Expected 3 arguments!");
	}

	int fd1 = open(argv[2], O_RDONLY);
	if (fd1 < 0) {
		err(2, "Error occurred while opening the second file for reading!");
	}

	int fd2 = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd2 < 0) {
		err(3, "Error occurred while opening the third file for writing!");
	}

	int fd_patch = open(argv[1], O_RDONLY);
	if (fd_patch < 0) {
		err(4, "Error occurred while opening the patch file for reading!");
	}
	
	struct header head;
	if (read(fd_patch, &head, sizeof(head)) < 0) {
		err(5, "Error occurred while reading the header of the patch file!");
	}

	if (head.header_version != 0x01) {
		err(6, "Wrong header version!");
	}

	if (head.data_version == 0x00) {
	
		uint8_t byte;
		transfer_file(&byte, sizeof(byte), fd1, fd2);	
		fd2 = open(argv[3], O_RDWR);
		if (fd2 < 0) {
			err(21, "Error opening the second file for reading and writing!");
		}
		patch_version_0(fd_patch, fd2, head.count);

		close(fd_patch);
		close(fd2);

	} else if (head.data_version == 0x01) {
		uint16_t byte;
		transfer_file(&byte, sizeof(byte), fd1, fd2);

		fd2 = open(argv[3], O_RDWR);
		if (fd2 < 0) {
			err(22, "Error opening the second file for reading and writing!");
		}
		patch_version_1(fd_patch, fd2, head.count);

		close(fd_patch);
		close(fd2);

	} else {
		err(23, "Error with data version!");
	}

	return 0;
}
