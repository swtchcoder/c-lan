#include <stdio.h>
#include <winsock2.h>

static const WORD version = MAKEWORD(2, 2);

int
main(void)
{
	WSADATA wsadata;
	if (WSAStartup(version, &wsadata)) {
		fprintf(stderr, "Missing the winsock dll");
		return 1;
	}
	puts("Hello world!");
	WSACleanup();
	return 0;
}