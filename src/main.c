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
	struct sockaddr_in addr = {
		AF_INET,
		htons(6969),
		htonl(INADDR_ANY)
	};

	int socket = create_listener(addr);

	if (socket == -1) {
		perror("Failed to create listener!");
		return EXIT_FAILURE;
	}

	printf("Listening...\n");

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
