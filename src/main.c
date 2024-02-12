#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/sendfile.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int get_port_from_string(char *port_str) {
	int c_idx = 0;
	while (port_str[c_idx] != '\0') {
		if (!isdigit(port_str[c_idx])) {
			return -1;
		}

		c_idx++;
	}

	return atoi(port_str);
}

int create_listener(struct sockaddr_in addr) {
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s == -1) {
		perror("Failed to create socket!");
		return -1;
	}

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Bind failed!");
		close(s);
		return -1;
	}

	if (listen(s, 10) == -1) {
		perror("Listen failed!");
		close(s);
		return -1;
	}

	return s;
}

int main(int argc, char *argv[]) {
	int port = 6969;

	for (int i = 1; i < argc; i++) {
		if (strcmp("--port", argv[i]) == 0) {
			if (i + 1 >= argc)
				break;

			char *port_str = argv[i + 1];

			port = get_port_from_string(port_str);

			if (port == -1) {
				printf("Invalid port!\n");
				return EXIT_FAILURE;
			}
		}
	}

	struct sockaddr_in addr = {
		AF_INET,
		htons(port),
		htonl(INADDR_ANY)
	};

	int socket = create_listener(addr);

	if (socket == -1) {
		perror("Failed to create listener!");
		return EXIT_FAILURE;
	}

	printf("Listening on http://localhost:%d/\n", port);

	while (true) {
		int c = accept(socket, NULL, NULL);

		if (c == -1) {
			perror("Accept failed!");
			continue;
		}

		char buff[256];
		recv(c, &buff, sizeof(buff), 0);

		char *p = buff + 5;
		*strchr(p, ' ') = 0;
		int f = open(p, O_RDONLY);

		sendfile(c, f, 0, 256);
		close(f);
		close(c);
	}

	close(socket);
	return EXIT_SUCCESS;
}
