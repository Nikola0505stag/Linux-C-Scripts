#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        errx(1, "Invalid number of arguments!");
    }

    if (strcmp(argv[1], "-c") == 0) {
        if (argc != 3) {
            errx(2, "Invalid number of arguments for -c option!");
        }

        if (strlen(argv[2]) == 1) {
            int num = argv[2][0] - '0';

            char buff;
            ssize_t read_result;
            int count = 0;

            while ((read_result = read(0, &buff, sizeof(buff))) > 0) {
                if (buff == '\n') {
                    if (write(1, &buff, sizeof(buff)) < 0) {
                        err(3, "Error writing newline!");
                    }
                    count = 0;
                    continue;
                }

                count++;
                if (count == num) {
                    if (write(1, &buff, sizeof(buff)) < 0) {
                        err(4, "Error writing character!");
                    }
                }
            }

            if (read_result < 0) {
                err(5, "Error reading from standard input!");
            }

        } else if (strlen(argv[2]) == 3 && argv[2][1] == '-') {
            int first_num = argv[2][0] - '0';
            int second_num = argv[2][2] - '0';

            char buff;
            ssize_t read_result;
            int count = 0;

            while ((read_result = read(0, &buff, sizeof(buff))) > 0) {
                if (buff == '\n') {
                    if (write(1, &buff, sizeof(buff)) < 0) {
                        err(6, "Error writing newline!");
                    }
                    count = 0;
                    continue;
                }

                count++;
                if (count >= first_num && count <= second_num) {
                    if (write(1, &buff, sizeof(buff)) < 0) {
                        err(7, "Error writing character!");
                    }
                }
            }

            if (read_result < 0) {
                err(8, "Error reading from standard input!");
            }
        } else {
            errx(9, "Invalid range format for -c!");
        }

    } else if (strcmp(argv[1], "-d") == 0) {
        if (argc != 5) {
            errx(10, "Expected exactly 5 arguments for -d option!");
        } 
        if (strcmp(argv[3], "-f") != 0) {
            errx(11, "Expected -f as third option!");
        }
        
        char delim = argv[2][0];
        int first_num = 0;
        int second_num = 0;

        if (strlen(argv[4]) == 1) {
            first_num = argv[4][0] - '0';
            second_num = first_num;
        } else if (strlen(argv[4]) == 3 && argv[4][1] == '-') {
            first_num = argv[4][0] - '0';
            second_num = argv[4][2] - '0';
        } else {
            errx(12, "Invalid range format for -f!");
        }

        char buff;
        ssize_t read_result;
        int count = 1; 

        while ((read_result = read(0, &buff, sizeof(buff))) > 0) {
            if (buff == '\n') {
                if (write(1, &buff, sizeof(buff)) < 0) {
                    err(13, "Error writing newline!");
                }
                count = 1; 
                continue;
            }

            if (buff == delim) {
                if (count >= first_num && count < second_num) {
                    if (write(1, &buff, sizeof(buff)) < 0) {
                        err(14, "Error writing delimiter!");
                    }
                }
                count++;
            } else {
                if (count >= first_num && count <= second_num) {
                    if (write(1, &buff, sizeof(buff)) < 0) {
                        err(15, "Error writing character!");
                    }
                }
            }
        }
        
        if (read_result < 0) {
            err(16, "Error reading from standard input!");
        }

    } else {
        errx(17, "Unknown option! Expected -c or -d.");
    }

    return 0;
}
