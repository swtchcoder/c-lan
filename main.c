#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define HOST "localhost"
#define PORT "50000"

static int loop(void);

static SOCKET server_tcp_s;

int
main(void)
{
	const WORD version = MAKEWORD(2, 2);
	WSADATA wsadata;
	struct addrinfo *result, hints;
	static SOCKET server_udp_s;
	int code;
	if (WSAStartup(version, &wsadata)) {
		fprintf(stderr, "Missing the winsock dll");
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	code = getaddrinfo(HOST, PORT, &hints, &result);
	if (code != 0) {
		fprintf(stderr, "getaddrinfo() error: %d\n", code);
		WSACleanup();
		return 1;
	}
	server_tcp_s = socket(result->ai_family, SOCK_STREAM, IPPROTO_TCP);
	if (server_tcp_s == INVALID_SOCKET) {
		code = WSAGetLastError();
		fprintf(stderr, "socket() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		return 1;
	}
	code = bind( server_tcp_s, result->ai_addr, (int)result->ai_addrlen);
	if (code == SOCKET_ERROR) {
		code = WSAGetLastError();
		fprintf(stderr, "bind() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		return 1;
	}
	code = listen(server_tcp_s, SOMAXCONN);
	if (code == SOCKET_ERROR) {
		code = WSAGetLastError();
		fprintf(stderr, "listen() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		return 1;
	}
	server_udp_s = socket(result->ai_family, SOCK_DGRAM, IPPROTO_UDP);
	if (server_udp_s == INVALID_SOCKET) {
		code = WSAGetLastError();
		fprintf(stderr, "socket() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		return 1;
	}
	code = bind( server_udp_s, result->ai_addr, (int)result->ai_addrlen);
	if (code == SOCKET_ERROR) {
		code = WSAGetLastError();
		fprintf(stderr, "bind() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		return 1;
	}
	while (loop());
	WSACleanup();
	freeaddrinfo(result);
	closesocket(server_tcp_s);
	closesocket(server_udp_s);
	return 0;
}

static int
loop(void)
{
	SOCKET client_s;
	struct sockaddr_in client_addr;
	int addrlen = sizeof(client_addr);
	char ip[INET_ADDRSTRLEN];
	int code;
	int port;
	client_s = accept(server_tcp_s, (struct sockaddr *)&client_addr, &addrlen);
	if (client_s == INVALID_SOCKET) {
		code = WSAGetLastError();
		fprintf(stderr, "accept() error: %d\n", code);
		return 0;
	}
	inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
	port = ntohs(client_addr.sin_port);
	closesocket(client_s);
	printf("Received request from %s:%d\n", ip, port);
	return 1;
}