#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

int compare(const void* a, const void* b) {
    uint32_t va = *(const uint32_t*)a;
    uint32_t vb = *(const uint32_t*)b;

    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        errx(1, "Expected two arguments!");
    }

    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) {
        err(2, "Error occurred while opening first file!");
    }

    size_t bytes = sizeof(uint32_t) * 4194304 / 2;
    uint32_t* first_nums = malloc(bytes);

    if (first_nums == NULL) {
        err(3, "Error allocating memory!");
    }

    ssize_t bytes_read1 = read(fd1, first_nums, bytes);
    if (bytes_read1 < 0) {
        err(4, "Error reading first half!");
    }

    size_t count1 = bytes_read1 / sizeof(uint32_t);

    if ((size_t)bytes_read1 < bytes) {
        qsort(first_nums, count1, sizeof(uint32_t), compare);

        int fd2 = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
        if (fd2 < 0) {
            err(5, "Error occurred while opening the second file!");
        }
        
        if (write(fd2, first_nums, bytes_read1) < 0) {
            err(6, "Error writing the nums to the second file!");
        }
        
        free(first_nums);
        close(fd1);
        close(fd2);
    } 
    else {
        qsort(first_nums, count1, sizeof(uint32_t), compare);

        char temp_file1[] = "/tmp/file1_XXXXXX";
        int temp_fd1 = mkstemp(temp_file1);
        if (temp_fd1 < 0) {
            err(7, "Error occurred while creating the first temp file!");
        }
        
        if (write(temp_fd1, first_nums, bytes) < 0) {
            err(8, "Error writing the nums to the first temp file!");
        }

        free(first_nums);

        uint32_t* second_nums = malloc(bytes);
        if (second_nums == NULL) {
            err(9, "Error allocating memory!");
        }
        
        ssize_t bytes_read2 = read(fd1, second_nums, bytes);
        if (bytes_read2 < 0) {
            err(10, "Error occurred while reading from first file!");
        }

        size_t count2 = bytes_read2 / sizeof(uint32_t);
        qsort(second_nums, count2, sizeof(uint32_t), compare);
        
        char temp_file2[] = "/tmp/file2_XXXXXX";
        int temp_fd2 = mkstemp(temp_file2);
        if (temp_fd2 < 0) {
            err(11, "Error creating the second temp file!");
        }

        if (write(temp_fd2, second_nums, bytes_read2) < 0) {
            err(12, "Error writing in the second temp file!");
        }

        free(second_nums);
        close(fd1);

        if (lseek(temp_fd1, 0, SEEK_SET) < 0 || lseek(temp_fd2, 0, SEEK_SET) < 0) {
            err(24, "Error seeking to the beginning of temp files!");
        }

        int fd2 = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
        if (fd2 < 0) {
            err(13, "Error occurred while opening the second file!");
        }

        uint32_t first_num, second_num;
        ssize_t read1_result = 0, read2_result = 0;
        int has_first = 0, has_second = 0;

        while (1) {
            if (!has_first) {
                read1_result = read(temp_fd1, &first_num, sizeof(first_num));
                if (read1_result > 0) {
                    has_first = 1;
                } else if (read1_result < 0) {
                    err(14, "Error reading from first temp file!");
                }
            }

            if (!has_second) {
                read2_result = read(temp_fd2, &second_num, sizeof(second_num));
                if (read2_result > 0) {
                    has_second = 1;
                } else if (read2_result < 0) {
                    err(15, "Error reading from second temp file!");
                }
            }

            if (has_first && has_second) {
                if (first_num <= second_num) {
                    if (write(fd2, &first_num, sizeof(first_num)) < 0) {
                        err(16, "Error writing the num in the second file!");
                    }
                    has_first = 0;
                } else {
                    if (write(fd2, &second_num, sizeof(second_num)) < 0) {
                        err(18, "Error writing the num in the second file!");
                    }
                    has_second = 0;
                }
            } else {
                break;
            }
        }

        if (has_first) {
            if (write(fd2, &first_num, sizeof(first_num)) < 0) {
                err(20, "Error writing the num in the second file!");
            }   
            while ((read1_result = read(temp_fd1, &first_num, sizeof(first_num))) > 0) {
                if (write(fd2, &first_num, sizeof(first_num)) < 0) {
                    err(20, "Error writing the num in the second file!");
                }   
            }
            if (read1_result < 0) {
                err(21, "Error reading from first temp file!");
            }
        }

        if (has_second) {
            if (write(fd2, &second_num, sizeof(second_num)) < 0) {
                err(22, "Error writing the num in the second file!");
            }   
            while ((read2_result = read(temp_fd2, &second_num, sizeof(second_num))) > 0) {
                if (write(fd2, &second_num, sizeof(second_num)) < 0) {
                    err(22, "Error writing the num in the second file!");
                }   
            }
            if (read2_result < 0) {
                err(23, "Error reading from second temp file!");
            }
        }

        close(temp_fd1);
        unlink(temp_file1);
        close(temp_fd2);
        unlink(temp_file2);
        close(fd2);
    }

    return 0;
}
