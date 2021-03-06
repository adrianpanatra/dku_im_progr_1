#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char *buffer;
char read_buffer[1024];

int main(int argc, char *argv[])
{
	int fd_sock, cli_sock;
	int port_num, ret, len;
	struct sockaddr_in addr;
	size_t getline_len;

	// arg parsing
	if (argc != 3) {
		printf("usage: cli srv_ip_addr port\n");
		return 0;
	}
	port_num = atoi(argv[2]);

	// socket creation
	fd_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_sock == -1) {
		perror("socket");
		return 0;
	}

	// addr binding, and connect
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_num); // using port num
	inet_pton(AF_INET, argv[1], &addr.sin_addr);

	ret = connect(fd_sock, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1) {
		perror("connect");
		close(fd_sock);
		return 0;
	}

	while (1) {
		buffer = NULL;
		printf("send$ ");

		// read user imput into the buffer
		ret = getline(&buffer, &getline_len, stdin);
		if (ret == -1) { // EOF
			perror("getline");
			close(fd_sock);
			break;
		}
		len = strlen(buffer);
		if(buffer[len-1] == '\n')
		{
			buffer[len-1] = '\0';
			len--;
		}
		if (len == 0) {
			free(buffer);
			continue;
		}

		// send the buffer
		send(fd_sock, buffer, len, 0);
		free(buffer);

		// read echo
		memset(read_buffer, 0, sizeof(read_buffer));
		len = recv(fd_sock, read_buffer, sizeof(read_buffer), 0);
		if (len < 0)
			break;
		printf("server responded: %s\n", read_buffer);
		fflush(NULL);
	}

	// bye-bye
	close(fd_sock);
	return 0;
}
