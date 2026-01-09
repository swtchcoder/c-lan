#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define HOST "localhost"
#define PORT "50000"

int
main(void)
{
	const WORD version = MAKEWORD(2, 2);
	WSADATA wsadata;
	struct addrinfo *result = NULL, hints;
	SOCKET server_socket = INVALID_SOCKET;
	int code;
	if (WSAStartup(version, &wsadata)) {
		fprintf(stderr, "Missing the winsock dll");
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	code = getaddrinfo(HOST, PORT, &hints, &result);
	if (code != 0) {
		fprintf(stderr, "getaddrinfo() error: %d\n", code);
		WSACleanup();
		return 1;
	}
	server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (server_socket == INVALID_SOCKET) {
		code = WSAGetLastError();
		fprintf(stderr, "socket() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		return 1;
	}
	code = bind( server_socket, result->ai_addr, (int)result->ai_addrlen);
	if (code == SOCKET_ERROR) {
		code = WSAGetLastError();
		fprintf(stderr, "bind() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		return 1;
	}
	code = listen( server_socket, SOMAXCONN );
	if (code == SOCKET_ERROR) {
		code = WSAGetLastError();
		fprintf(stderr, "listen() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		return 1;
	}
	puts("Hello world!");
	WSACleanup();
	freeaddrinfo(result);
	closesocket(server_socket);
	return 0;
}