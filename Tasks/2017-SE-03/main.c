#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>

struct Triple{
	uint16_t offset;
	uint8_t orig_byte;
	uint8_t new_byte;
} __attribute__((packed));

int main(int argc, char* argv[]) {
	if (argc != 4) {
		err(1, "Expected 3 arguments!");
	}

	int fd_f1 = open(argv[2], O_RDONLY);
	if (fd_f1 < 0) {
		err(2, "Error opening file!");
	}

	int fd_f2 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_f2 < 0) {
		err(3, "Error opening second file!");
	}

	uint8_t copy_buf;
    ssize_t read_sz;
    while ((read_sz = read(fd_f1, &copy_buf, 1)) > 0) {
        if (write(fd_f2, &copy_buf, 1) < 0) {
            err(4, "Error writing during copy to %s", argv[3]);
        }
    }
    if (read_sz < 0) {
        err(5, "Error reading from %s during copy", argv[2]);
    }

    close(fd_f1);
    close(fd_f2);

    int fd_patch = open(argv[1], O_RDONLY);
    if (fd_patch < 0) {
        err(6, "Error opening patch file %s", argv[1]);
    }

    fd_f2 = open(argv[3], O_RDWR);
    if (fd_f2 < 0) {
        err(7, "Error opening %s for patching", argv[3]);
    }

	struct Triple triple;
    
    while ((read_sz = read(fd_patch, &triple, sizeof(struct Triple))) > 0) {
        if (read_sz != sizeof(struct Triple)) {
            errx(8, "Corrupted patch file structure");
        }

        if (lseek(fd_f2, triple.offset, SEEK_SET) < 0) {
            err(9, "Offset %d does not exist in %s", triple.offset, argv[3]);
        }

        uint8_t current_byte;
        ssize_t f2_read = read(fd_f2, &current_byte, 1);
        
        if (f2_read == 0) {
            errx(10, "Offset %d is beyond file size of %s", triple.offset, argv[3]);
        } else if (f2_read < 0) {
            err(11, "Error reading from %s during patch check", argv[3]);
        }

        if (current_byte != triple.orig_byte) {
            errx(12, "Original byte mismatch at offset %d", triple.offset);
        }

        if (lseek(fd_f2, -1, SEEK_CUR) < 0) {
            err(13, "Error adjusting position in %s", argv[3]);
        }

        if (write(fd_f2, &triple.new_byte, 1) < 0) {
            err(14, "Error writing patched byte to %s", argv[3]);
        }
    }

    if (read_sz < 0) {
        err(15, "Error reading from patch file %s", argv[1]);
    }

    close(fd_patch);
    close(fd_f2);

    return 0;
}
