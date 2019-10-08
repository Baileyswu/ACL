using namespace std;
#ifdef _WIN32
#pragma comment(lib, "pthreadVC2.lib")
#endif

#define HAVE_STRUCT_TIMESPEC

#include "XSocket.h"
#include "nfa.h"
#include <iostream>
#include <pthread.h>

using namespace std;
using namespace XSOCKET;
using namespace NFA;

XSocket skt;

pthread_mutex_t plug_mutex = PTHREAD_MUTEX_INITIALIZER;

void* Handle_Thread(void* ptr)
{
	printf("%s line:%d\n", __func__, __LINE__);
	int client_sockid = *((int*)ptr);
	char recv_buf[BUFSIZ];
	char send_buf[BUFSIZ];
	int buf_len;
	int flag = SUCCESS;
	int wait_time = 0;
	Nfa nfa;
	while (flag == SUCCESS)
	{
		flag = skt.Receive(client_sockid, recv_buf, buf_len);
		cout << "flag: " << flag << endl;
		if (flag == WSAETIMEDOUT && wait_time + RECV_CIRCLE_TIME < TIME_OUT_LIMIT)
		{
			wait_time += RECV_CIRCLE_TIME;
			printf("wait client %d for %d seconds, send heartbeat\n", client_sockid, wait_time);
			skt.Make_Heart_Beat(send_buf, buf_len);
			flag = skt.Send(client_sockid, send_buf, buf_len);
			continue;
		}
		wait_time = 0;
		if (flag == HEARTBEAT || flag == NOT_RIGHT_PROTOCAL || flag == PKG_TOO_LONG)
		{
			flag = SUCCESS;
			continue;
		}
		if (flag != SUCCESS) {
			break;
		}
		if (nfa.Parse_Data(recv_buf, send_buf, buf_len) == SUCCESS)
			flag = skt.Send(client_sockid, send_buf, buf_len);
	}
	skt.Close_Client(client_sockid);
	pthread_exit(0);
	return nullptr;
}

void* Listen_Thread(void* arg)
{
	printf("%s line:%d\n", __func__, __LINE__);
	while (1)
	{
		pthread_t c_thread;
		int client_sockid;
		SOCKADDR client_addr;
		if (skt.Accept(client_sockid, client_addr) != SUCCESS)
			continue;
		pthread_create(&c_thread, NULL, Handle_Thread, &client_sockid);
		pthread_detach(c_thread);
	}
	pthread_exit(0);
	return nullptr;
}

int main()
{
	printf("%s line:%d\n", __func__, __LINE__);
	void* main_id;
	pthread_t listen_id;

	if (skt.Start_Server() != SUCCESS)
	{
		skt.Close_Server();
		return XSOCKET_ERROR;
	}

	pthread_create(&listen_id, NULL, Listen_Thread, NULL);
	pthread_join(listen_id, &main_id);

	skt.Close_Server();

	return SUCCESS;
}