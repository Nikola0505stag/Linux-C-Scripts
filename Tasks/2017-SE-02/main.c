#include <fcntl.h>    // За O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC...
#include <unistd.h>   // За open, read, write, close, lseek, ...
#include <err.h>      // За err, errx, warn...
#include <string.h>   // За strcmp 
#include <stdio.h>    // За snprintf 

void catFileNumed(int fd, int* num_line) {
	if (fd < 0) {
		err(4, "Error openning file for reading");
	}

	int exit_status;
	char symbol;
	char num_buf[20];

	(*num_line)++;
	int len = snprintf(num_buf, sizeof(num_buf), "%d: ", *num_line);
	if (write(1, num_buf, len) < 0) {
		err(7, "Error writing in default exit");
	}

	while ((exit_status = read(fd, &symbol, sizeof(symbol))) > 0) {
		if (symbol == '\n') {
			if (write(1, &symbol, sizeof(symbol)) < 0) {
				err(9, "Error writing in the default exit");
			}
        
			(*num_line)++;
			len = snprintf(num_buf, sizeof(num_buf), "%d: ", *num_line);
			if (write(1, num_buf, len) < 0) {
				err(8, "Error writing in default exit");
			}   
		} else {
			if (write(1, &symbol, sizeof(symbol)) < 0) {
				err(5, "Error writing in the default exit");
			}
		}
	}
	
	if (exit_status < 0) {
		err(6, "Error reading from file");
	}

	if (fd != 0){
		close(fd);
	} 
} 

void catFileNotNumed(int fd) {
	if (fd < 0) {
		err(1, "Error openning file for reading!");
	}

	int exit_status;
	char symbol;

	while ((exit_status = read(fd, &symbol, sizeof(symbol))) > 0) {
		if (write(1, &symbol, sizeof(symbol)) < 0) {
			err(2, "Error writing in the default exit");
		}
	}

	if (exit_status < 0) {
		err(3, "Error reading from file!");
	}

	if (fd != 0){
		close(fd);
	} 
}


int main(int argc, char* argv[]) {
	int num_line = 0;
	if (argc > 1) {
		
		if (strcmp(argv[1], "-n") == 0) { // when there IS -n
			for (int i = 2; i < argc; i++) {
				if (strcmp(argv[i], "-") != 0) {
					int fd = open(argv[i], O_RDONLY);
					catFileNumed(fd, &num_line);
				}
				else {
					catFileNumed(0, &num_line);	
				}
			}
		} else {
			for (int i = 1; i < argc; i++) { // when there ISN'T -n
				if (strcmp(argv[i], "-") != 0) {
					int fd = open(argv[i], O_RDONLY);
					catFileNotNumed(fd);
				}
				else {
					catFileNotNumed(0);
				}
			}	
		}

	} else {
		catFileNotNumed(0);
	}

	return 0;
} 

