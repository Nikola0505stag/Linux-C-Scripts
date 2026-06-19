#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>

struct Interval {
	uint16_t start_idx;
	uint16_t length;
};

// Един комплект сесъстои от точно 4 интервала, позиционно отнасящи се към файловете
struct Kit {
	struct Interval post_int;
	struct Interval pre_int; 
	struct Interval in_int;  
	struct Interval suf_int; 
};

uint16_t get_element_count(int fd) {
	char header[16];
	if (read(fd, header, 16) != 16) {
		errx(2, "Corrupted header or file too small!");
	}
	return *(uint16_t*)(&header[4]);
}

int main(int argc, char* argv[]) {
	if (argc != 7) {
		errx(1, "Invalid arguments count! Expected 6 file names.");
	}

	int fd_affix   = open(argv[1], O_RDONLY);
	int fd_postfix = open(argv[2], O_RDONLY);
	int fd_prefix  = open(argv[3], O_RDONLY);
	int fd_infix   = open(argv[4], O_RDONLY);
	int fd_suffix  = open(argv[5], O_RDONLY);
	int fd_cruc   = open(argv[6], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (fd_affix < 0 || fd_postfix < 0 || fd_prefix < 0 || fd_infix < 0 || fd_suffix < 0 || fd_cruc < 0) {
		err(3, "Error opening one or more files!");
	}

	uint16_t kits_count = get_element_count(fd_affix);
	get_element_count(fd_postfix);
	get_element_count(fd_prefix);
	get_element_count(fd_infix);
	get_element_count(fd_suffix);
	char empty_header[16] = {0};
	if (write(fd_cruc, empty_header, 16) != 16) {
		err(4, "Error writing header to crucifixus!");
	}

	for (int k = 0; k < kits_count; k++) {
		struct Kit current_kit;

		if (read(fd_affix, &current_kit, sizeof(struct Kit)) != sizeof(struct Kit)) {
			errx(5, "Error reading kit from affix file!");
		}

		if (lseek(fd_postfix, 16 + (current_kit.post_int.start_idx * sizeof(uint32_t)), SEEK_SET) < 0) {
			err(6, "lseek failed for postfix");
		}
		for (int i = 0; i < current_kit.post_int.length; i++) {
			uint32_t value;
			if (read(fd_postfix, &value, sizeof(uint32_t)) != sizeof(uint32_t)) {
				errx(7, "Read error from postfix");
			}
			if (write(fd_cruc, &value, sizeof(uint32_t)) != sizeof(uint32_t)) {
				err(8, "Write error to crucifixus");
			}
		}

		if (lseek(fd_prefix, 16 + (current_kit.pre_int.start_idx * sizeof(uint8_t)), SEEK_SET) < 0) {
			err(6, "lseek failed for prefix");
		}
		for (int i = 0; i < current_kit.pre_int.length; i++) {
			uint8_t value;
			if (read(fd_prefix, &value, sizeof(uint8_t)) != sizeof(uint8_t)) {
				errx(7, "Read error from prefix");
			}
			if (write(fd_cruc, &value, sizeof(uint8_t)) != sizeof(uint8_t)) {
				err(8, "Write error to crucifixus");
			}
		}

		if (lseek(fd_infix, 16 + (current_kit.in_int.start_idx * sizeof(uint16_t)), SEEK_SET) < 0) {
			err(6, "lseek failed for infix");
		}
		for (int i = 0; i < current_kit.in_int.length; i++) {
			uint16_t value;
			if (read(fd_infix, &value, sizeof(uint16_t)) != sizeof(uint16_t)) {
				errx(7, "Read error from infix");
			}
			if (write(fd_cruc, &value, sizeof(uint16_t)) != sizeof(uint16_t)) {
				err(8, "Write error to crucifixus");
			}
		}

		if (lseek(fd_suffix, 16 + (current_kit.suf_int.start_idx * sizeof(uint64_t)), SEEK_SET) < 0) {
			err(6, "lseek failed for suffix");
		}
		for (int i = 0; i < current_kit.suf_int.length; i++) {
			uint64_t value;
			if (read(fd_suffix, &value, sizeof(uint64_t)) != sizeof(uint64_t)) {
				errx(7, "Read error from suffix");
			}
			if (write(fd_cruc, &value, sizeof(uint64_t)) != sizeof(uint64_t)) {
				err(8, "Write error to crucifixus");
			}
		}
	}

	close(fd_affix);
	close(fd_postfix);
	close(fd_prefix);
	close(fd_infix);
	close(fd_suffix);
	close(fd_cruc);

	return 0;
}
