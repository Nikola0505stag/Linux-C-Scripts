#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc != 4) {
		errx(1, "Usage: %s <prog1> <prog2> <prog3>", argv[0]);
	}

	int to_p1[2], from_p1[2];
	int to_p2[2], from_p2[2];
	int to_p3[2], from_p3[2];

	if (pipe(to_p1) < 0 || pipe(from_p1) < 0 ||
		pipe(to_p2) < 0 || pipe(from_p2) < 0 ||
		pipe(to_p3) < 0 || pipe(from_p3) < 0) {
		err(2, "Failed to create pipes");
	}


	pid_t pid1 = fork();
	if (pid1 < 0) {
		err(3, "First fork failed");
	}
	if (pid1 == 0) {
		dup2(to_p1[0], 0);
		dup2(from_p1[1], 1);
		
		close(to_p1[0]); 
		close(to_p1[1]); 
		close(from_p1[0]); 
		close(from_p1[1]);
		close(to_p2[0]);
		close(to_p2[1]);
		close(from_p2[0]);
		close(from_p2[1]);
		close(to_p3[0]);
		close(to_p3[1]);
		close(from_p3[0]);
		close(from_p3[1]);

		execl(argv[1], argv[1], (char*)NULL);
		err(4, "Error executing program 1");
	}

	pid_t pid2 = fork();
	if (pid2 < 0) {
		err(5, "Second fork failed");
	}
	if (pid2 == 0) {
		dup2(to_p2[0], 0);
		dup2(from_p2[1], 1);
		
		close(to_p1[0]);
		close(to_p1[1]);
		close(from_p1[0]);
		close(from_p1[1]);
		close(to_p2[0]);
		close(to_p2[1]);
		close(from_p2[0]);
		close(from_p2[1]);
		close(to_p3[0]);
		close(to_p3[1]);
		close(from_p3[0]);
		close(from_p3[1]);

		execl(argv[2], argv[2], (char*)NULL);
		err(6, "Error executing program 2");
	}

	pid_t pid3 = fork();
	if (pid3 < 0) err(7, "Third fork failed");
	if (pid3 == 0) {
		dup2(to_p3[0], 0);
		dup2(from_p3[1], 1);
		
		close(to_p1[0]); close(to_p1[1]); close(from_p1[0]); close(from_p1[1]);
		close(to_p2[0]); close(to_p2[1]); close(from_p2[0]); close(from_p2[1]);
		close(to_p3[0]); close(to_p3[1]); close(from_p3[0]); close(from_p3[1]);

		execl(argv[3], argv[3], (char*)NULL);
		err(8, "Error executing program 3");
	}

	close(to_p1[0]); close(from_p1[1]);
	close(to_p2[0]); close(from_p2[1]);
	close(to_p3[0]); close(from_p3[1]);

	unsigned char current_byte;
	int prev_byte = -1;

	while (read(0, &current_byte, 1) > 0) {
		
		if (prev_byte != -1 && (unsigned char)prev_byte == current_byte &&
			current_byte != 0x00 && current_byte != 0x55 && current_byte != 0x7D && current_byte != 0xFF) {
			
			char fake = 0x55;
			write(to_p1[1], &fake, 1);
			write(to_p2[1], &fake, 1);
			write(to_p3[1], &fake, 1);

			unsigned char ack;
			read(from_p1[0], &ack, 1);
			read(from_p2[0], &ack, 1);
			read(from_p3[0], &ack, 1);
		}

		if (current_byte == 0x00 || current_byte == 0x55 || current_byte == 0x7D || current_byte == 0xFF) {
			char esc = 0x7D;
			char xored = current_byte ^ 0x20;

			write(to_p1[1], &esc, 1); write(to_p1[1], &xored, 1);
			write(to_p2[1], &esc, 1); write(to_p2[1], &xored, 1);
			write(to_p3[1], &esc, 1); write(to_p3[1], &xored, 1);
		} else {
			write(to_p1[1], &current_byte, 1);
			write(to_p2[1], &current_byte, 1);
			write(to_p3[1], &current_byte, 1);
		}

		unsigned char ack1, ack2, ack3;
		read(from_p1[0], &ack1, 1);
		read(from_p2[0], &ack2, 1);
		read(from_p3[0], &ack3, 1);

		if (ack1 != 0x01 || ack2 != 0x01 || ack3 != 0x01) {
			errx(9, "Error: Mode did not return 0x01 confirmation");
		}

		prev_byte = current_byte;
	}

	close(to_p1[1]); close(from_p1[0]);
	close(to_p2[1]); close(from_p2[0]);
	close(to_p3[1]); close(from_p3[0]);

	wait(NULL); wait(NULL); wait(NULL);

	return 0;
}
