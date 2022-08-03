#include "server.h"

void tcp_open()
{
	int serv_sock;
	int client_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;

	serv_sock = socket(PF_INET, SOCK_STREAM,0);
	if(serv_sock == -1)
		printf("socket error\n");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DEFAULT_PORT);

	if(bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		printf("bind error\n");

	if(listen(serv_sock,5) == -1)
		printf("listen error\n");

	socklen_t clnt_addr_size = sizeof(client_addr);
	client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);
	while (1)
	{
		char message[20];
		if (read(client_sock, message, sizeof(message)) <= 0)
		{
			client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);
			read(client_sock, message, sizeof(message));
		}
		
		printf("%s\n", message);
	}
	close(serv_sock);
	close(client_sock);
}

int main()
{
	tcp_open();
	// pthread_t p_thread;
	// pthread_create(&p_thread, NULL, tcp_open, NULL);
	// while (1);
	return 0;
}