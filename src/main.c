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

int main(int argc, char *argv[]) {
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s == -1) {
		perror("Failed to create socket!");
		return EXIT_FAILURE;
	}

	struct sockaddr_in s_addr = {
		AF_INET,
		htons(6969),
		htonl(INADDR_ANY)
	};

	if (bind(s, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
		perror("Bind failed!");
		close(s);
		return EXIT_FAILURE;
	}

	if (listen(s, 10) == -1) {
		perror("Listen failed!");
		close(s);
		return EXIT_FAILURE;
	}

	printf("Listening...\n");

	while (true) {
		int c = accept(s, NULL, NULL);

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

	close(s);
	return EXIT_SUCCESS;
}
