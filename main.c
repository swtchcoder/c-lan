#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#define PORT "50000"
#define BUFFER_SIZE 4096

static DWORD WINAPI tcp_thread(LPVOID lpParam);
static DWORD WINAPI udp_thread(LPVOID lpParam);

static SOCKET server_tcp_s;
static SOCKET server_udp_s;
static int running = 1;

int
main(void)
{
	const WORD version = MAKEWORD(2, 2);
	WSADATA wsadata;
	struct addrinfo *result, hints;
	int code;
	HANDLE _tcp_thread, _udp_thread;
	if (WSAStartup(version, &wsadata)) {
		fprintf(stderr, "Missing the winsock dll");
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	code = getaddrinfo(NULL, PORT, &hints, &result);
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
	if (bind( server_tcp_s, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		code = WSAGetLastError();
		fprintf(stderr, "bind() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		closesocket(server_tcp_s);
		return 1;
	}
	if (listen(server_tcp_s, SOMAXCONN) == SOCKET_ERROR) {
		code = WSAGetLastError();
		fprintf(stderr, "listen() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		closesocket(server_tcp_s);
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
	if (bind( server_udp_s, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		code = WSAGetLastError();
		fprintf(stderr, "bind() error: %d\n", code);
		WSACleanup();
		freeaddrinfo(result);
		closesocket(server_udp_s);
		return 1;
	}
	_tcp_thread = CreateThread(NULL, 0, tcp_thread, NULL, 0, NULL);
	_udp_thread = CreateThread(NULL, 0, udp_thread, NULL, 0, NULL);
	WaitForSingleObject(_tcp_thread, INFINITE);
	WaitForSingleObject(_udp_thread, INFINITE);
	WSACleanup();
	freeaddrinfo(result);
	closesocket(server_tcp_s);
	closesocket(server_udp_s);
	return 0;
}

static DWORD WINAPI
tcp_thread(LPVOID lpParam)
{
	SOCKET client_s;
	struct sockaddr_in client_addr;
	int addrlen = sizeof(client_addr);
	char ip[INET_ADDRSTRLEN];
	int code;
	int port;
	while (running) {
		puts("(tcp) Waiting for connection");
		client_s = accept(server_tcp_s, (struct sockaddr *)&client_addr, &addrlen);
		if (client_s == INVALID_SOCKET) {
			code = WSAGetLastError();
			fprintf(stderr, "accept() error: %d\n", code);
			return 1;
		}
		inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
		port = ntohs(client_addr.sin_port);
		closesocket(client_s);
		printf("(tcp) Received request from %s:%d\n", ip, port);
	}
	return 0;
}

static DWORD WINAPI
udp_thread(LPVOID lpParam)
{
	int bytes;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	int addrlen = sizeof(client_addr);
	char ip[INET_ADDRSTRLEN];
	int port;
	while (running) {
		bytes = recvfrom(server_udp_s, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addrlen);
		if (bytes > 0) {
			inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
			port = ntohs(client_addr.sin_port);
			printf("(udp) Received %d bytes from %s:%d\n", bytes, ip, port);
		}
	}
	return 0;
}