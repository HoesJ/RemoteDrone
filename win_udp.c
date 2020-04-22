/*
	Simple UDP Server
*/

#include<stdio.h>
#include<winsock2.h>

#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data

int main()
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &amp; wsa) != 0)
	{
		printf(&quot; Failed.Error Code : %d&quot; , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf(&quot; Could not create socket : %d&quot;, WSAGetLastError());
	}
	printf(&quot; Socket created.\n&quot;);

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&amp; server, sizeof(server)) == SOCKET_ERROR)
	{
		printf(&quot; Bind failed with error code : %d&quot;, WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts(&quot; Bind done&quot;);

	//keep listening for data
	while (1)
	{
		printf(&quot; Waiting for data...&quot;);
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '&#92;&#48;', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &amp; si_other, &amp; slen)) == SOCKET_ERROR)
		{
			printf(&quot; recvfrom() failed with error code : %d&quot;, WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		printf(&quot; Received packet from %s:%d\n&quot; , inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf(&quot; Data: %s\n&quot;, buf);

		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &amp; si_other, slen) == SOCKET_ERROR)
		{
			printf(&quot; sendto() failed with error code : %d&quot;, WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}