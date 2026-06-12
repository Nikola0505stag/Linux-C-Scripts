#include <fcntl.h> // O_WRONLY, ...
#include <unistd.h> // read, write, lseek, ...
#include <err.h> // err, errx, ...
#include <stdint.h> // uint32_5, ...
#include <stdlib.h>
#include <sys/stat.h>

int compare(const void* a, const void* b) {
	uint32_t num_a = *(const uint32_t*)a;
	uint32_t num_b = *(const uint32_t*)b;

	if(num_a < num_b) {
		return -1;
	} else if (num_a > num_b) {
		return 1;
	}

	return 0;
}


int main(int argc, char* argv[]) {
	if (argc != 2) {
		err(1, "Expected 1 argument!");
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		err(2, "Error opening file!");
	}

	struct stat st;
	if (fstat(fd, &st) < 0) {
		err(3, "Error getting file stats!");
	}

	off_t total_size = st.st_size;
	size_t total_elements = total_size / sizeof(uint32_t);

	if (total_elements <= 1) {
		close(fd);
		return 0;
	}

	size_t max_chunk_elements = 20000000;
	uint32_t* buffer = malloc(max_chunk_elements * sizeof(uint32_t));
	if (buffer == NULL) {
		err(4, "Memory allocation failed");
	}

	const char* temp_file1 = "temp1.bin";
	const char* temp_file2 = "temp2.bin";

	int t1_fd = open(temp_file1, O_RDWR | O_CREAT | O_TRUNC, 0644);
	int t2_fd = open(temp_file2, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (t2_fd < 0 || t1_fd < 0) {
		err(5, "Error creating temporary files!");
	}

	size_t elements_t1 = 0;
	size_t elements_t2 = 0;

	size_t elements_left = total_elements;
	int write_to_t1 = 1;

	while (elements_left > 0) {
		size_t current_chunk_size = (elements_left < max_chunk_elements) ? elements_left : max_chunk_elements;
        ssize_t read_bytes = read(fd, buffer, current_chunk_size * sizeof(uint32_t));
		if (read_bytes < 0){
			err(6, "Error reading chunk from input file");
		} 

        qsort(buffer, current_chunk_size, sizeof(uint32_t), compare);

        if (write_to_t1) {
            if (write(t1_fd, buffer, read_bytes) < 0) err(7, "Error writing to temp1");
            elements_t1 += current_chunk_size;
            write_to_t1 = 0;
        } else {
            if (write(t2_fd, buffer, read_bytes) < 0) err(7, "Error writing to temp2");
            elements_t2 += current_chunk_size;
            write_to_t1 = 1;
        }

        elements_left -= current_chunk_size;
    }

    free(buffer);
    close(fd);

    if (lseek(t1_fd, 0, SEEK_SET) < 0 || lseek(t2_fd, 0, SEEK_SET) < 0) {
        err(8, "Error resetting temp files positions");
    }

    int out_fd = open(argv[1], O_WRONLY | O_TRUNC);
	if (out_fd < 0){
		err(9, "Error opening original file for writing");
	} 

    uint32_t val1, val2;
    ssize_t r1 = 0, r2 = 0;
    size_t p1 = 0, p2 = 0;

	if (p1 < elements_t1){
		r1 = read(t1_fd, &val1, sizeof(uint32_t));
	} 
	if (p2 < elements_t2) {
		 r2 = read(t2_fd, &val2, sizeof(uint32_t));
	}

    while (p1 < elements_t1 && p2 < elements_t2) {
        if (val1 <= val2) {
			if (write(out_fd, &val1, sizeof(uint32_t)) < 0){
				err(10, "Error writing during merge");
			}
            p1++;
            if (p1 < elements_t1) r1 = read(t1_fd, &val1, sizeof(uint32_t));
        } else {
            if (write(out_fd, &val2, sizeof(uint32_t)) < 0) err(10, "Error writing during merge");
            p2++;
            if (p2 < elements_t2) r2 = read(t2_fd, &val2, sizeof(uint32_t));
        }
    }

    while (p1 < elements_t1) {
        if (write(out_fd, &val1, sizeof(uint32_t)) < 0) err(10, "Error writing remaining data");
        p1++;
        if (p1 < elements_t1) r1 = read(t1_fd, &val1, sizeof(uint32_t));
    }

    while (p2 < elements_t2) {
        if (write(out_fd, &val2, sizeof(uint32_t)) < 0) err(10, "Error writing remaining data");
        p2++;
        if (p2 < elements_t2) r2 = read(t2_fd, &val2, sizeof(uint32_t));
    }

    if (r1 < 0 || r2 < 0) err(11, "Error during final read from temp files");

    close(t1_fd);
    close(t2_fd);
    close(out_fd);

    unlink(temp_file1);
    unlink(temp_file2);

    return 0;
}
