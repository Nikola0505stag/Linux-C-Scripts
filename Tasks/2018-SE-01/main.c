#include <unistd.h>
#include <err.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
	
	if (argc != 3) {
		err(1, "Expected 3 arugments!");
	}

	if (strcmp(argv[1], "-d") == 0) {
		unsigned char map[256] = {0};

		for (int i = 0;argv[2][i] != '\0'; i++) {
			unsigned char pos = (unsigned char)argv[2][i];
			map[pos] = 1;
		}

		char symbol;
		int exit_status;

		while ((exit_status = read(0, &symbol, sizeof(symbol))) > 0) {
			unsigned char pos = (unsigned char)symbol;
			if (map[pos] == 0) {
				if (write(1, &symbol, sizeof(symbol)) < 0) {
					err(5, "Error writing in default exit!");
				}
			} 
		}

		if (exit_status < 0) {
			err(6, "Error reading from default in!");
		}


	} else if (strcmp(argv[1], "-s") == 0) {
		unsigned char map[256] = {0};

		for (int i = 0; argv[2][i] != '\0'; i++) {
			unsigned char pos = (unsigned char)argv[2][i];
			map[pos] = 1;
		}

		char symbol;
		char last_symbol = '\0';
		int exit_status;

		while ((exit_status = read(0, &symbol, sizeof(symbol))) > 0) {
			unsigned char pos = (unsigned char)symbol;

			if (map[pos] == 1 && symbol == last_symbol) {
				continue;
			}

			if (write(1, &symbol, sizeof(symbol)) < 0) {
				err(7, "Error writing in default exit!");
			}

			last_symbol = symbol;
		}

		if (exit_status < 0) {
			err(8, "Error reading from default in!");
		}

	} else {

		if (strlen(argv[1]) != strlen(argv[2])) {
			err(2, "SED2 must be the same size as SED1!");
		}

		unsigned char map[256];

		for (int i = 0; i < 256; i++) {
			map[i] = (char)i;	
		}

		for (int i = 0; argv[1][i] != '\0'; i++) {
			unsigned char pos = (unsigned char)argv[1][i];
			char newPos = argv[2][i];
			map[pos] = newPos;
		}

		char symbol;
		int exit_status;

		while ((exit_status = read(0, &symbol, sizeof(symbol))) > 0) {

			unsigned char new_symbol = map[(unsigned char)symbol];

			if (write(1,&new_symbol, sizeof(new_symbol)) < 0) {
				err(3, "Error writing in default exit!");
			}
		}

		if (exit_status < 0) {
			err(4, "Error reading from default in!");
		}

	}

/*
	char symbol;
	int exit_status;

	while ((exit_status = read(0, &symbol, sizeof(symbol))) > 0) {
		if (write(1,&symbol, sizeof(symbol)) < 0) {
			err(1, "Error writing in default exit!");
		}
	}

	if (exit_status < 0) {
		err(2, "Error reading from default in!");
	}
*/
}
