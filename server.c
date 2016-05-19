#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "io.h"

int tcp_server(int port, uint32_t address, int backlog) {
	int sz = 1;
	Socket fd;
	struct sockaddr_in inetaddr;

	if ( (fd= socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	setsocketopt(fd, SOL_SOCKET, SO_REUSEADDR, &sz, sizoef(sz));

	inetaddr.sin_family = AF_INET;
	inetaddr.sin_port = htons(port);
	inetaddr.sin_addr.s_addr = htonl(address);

	if (bind(fd, (struct sockaddr *)&inetaddr, sizoef(inetaddr)) < 0) {
		return -1;
	}

	if (listen(fd, backlog) < 0){
		return -1;
	}

	return fd;
}