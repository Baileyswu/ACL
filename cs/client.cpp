#ifdef _WIN32
#pragma comment(lib, "pthreadVC2.lib")
#endif

#define HAVE_STRUCT_TIMESPEC

#include "XSocket.h"
#include "NProtocol.h"
#include <iostream>
#include <windows.h>
#include <time.h>
#include <pthread.h>

#define FILE_NAME "blocklist"
#define READ_ERR -80
using namespace std;
using namespace XSOCKET;

XSocket skt;
int send_time;

void Pack_Msg(char command, char* str, char* buf, int& slen) 
{
	N_MESSAGE* msg = Make_Message(PROTOCOL_TYPE, HEART_BEAT + 1, command, str);
	slen = msg->getsize();
	memcpy(buf, msg, slen);
}

int Decode_Msg(char* buf, int& len) 
{
	N_MESSAGE* msg = (N_MESSAGE*)buf;
	N_HEADER* head_ptr = &(msg->header);
	char* str = buf + sizeof(N_HEADER);

	if (head_ptr->protocol != PROTOCOL_TYPE)
		return XSOCKET_ERROR;

	if (head_ptr->pkgFlag == HEART_BEAT)
		return XSOCKET_ERROR;

	string outprint;
	switch (buf[len - 1]) {
	case 'y': outprint = "[success]"; break;
	case 'n': outprint = "[failed]"; break;
	case 'd': outprint = "[has done]"; break;
	default: outprint = "[unknown]";
	}
	switch (head_ptr->command) {
	case 'a': outprint = "Insert rule " + outprint; break;
	case 'q': outprint = "Find in rule " + outprint; break;
	case 'd': outprint = "Delete rule " + outprint;
	}
	cout << outprint << endl;
	return buf[len - 1];
}

int CurrentTime() 
{
	return time(0);
}

int Read_From_File(FILE* fp, char& command, char* str) 
{
	char* find;
	if (!feof(fp) && fgets(str, BUFSIZ, fp)) {
		command = 'a';
		find = strchr(str, '\n');
		if (find)
			*find = '\0';
		cout << command << " " << str << endl;
		return SUCCESS;
	}
	fclose(fp);
	return READ_ERR;
}

int Read_From_Keyboard(char& command, char* str) {
	cin >> command >> str;
	return SUCCESS;
}

void* Work_Thread(void* ptr)
{
	printf("%s line:%d\n", __func__, __LINE__);
	char command;
	char str[BUFSIZ];
	char send_buf[BUFSIZ];
	char recv_buf[BUFSIZ];
	int len;
	send_time = CurrentTime();
	FILE* pFile;
	pFile = fopen(FILE_NAME, "r");
	if (pFile == NULL) {
		perror("Error opening file");
		pthread_exit(0);
		return nullptr;
	}

	while (Read_From_File(pFile, command, str) == SUCCESS 
		|| Read_From_Keyboard(command, str) == SUCCESS) {
		Pack_Msg(command, str, send_buf, len);
		if (skt.Send(send_buf, len) == SUCCESS && skt.Receive(recv_buf, len) == SUCCESS) {
			Decode_Msg(recv_buf, len);
			send_time = CurrentTime();
			continue;
		}
	}
	pthread_exit(0);
	return nullptr;
}

void* HeartBeat_Thread(void* ptr)
{
	printf("%s line:%d\n", __func__, __LINE__);
	char command;
	char str[BUFSIZ];
	char send_buf[BUFSIZ];
	char recv_buf[BUFSIZ];
	int len;

	while (true) {
		if (send_time + RECV_CIRCLE_TIME < CurrentTime()) {
			skt.Make_Heart_Beat(send_buf, len);
			skt.Send(send_buf, len);
			send_time = CurrentTime();
		}
	}
	pthread_exit(0);
	return nullptr;
}

int main()
{
	printf("%s line:%d\n", __func__, __LINE__);
	void* main_id;
	pthread_t work_id;
	pthread_t heartbeat_id;

	if (skt.Start_Client() != SUCCESS) {
		skt.Close_Client();
		return XSOCKET_ERROR;
	}

	pthread_create(&work_id, NULL, Work_Thread, NULL);
	pthread_create(&heartbeat_id, NULL, HeartBeat_Thread, NULL);
	pthread_join(work_id, &main_id);
	pthread_join(heartbeat_id, &main_id);

	skt.Close_Client();

	return SUCCESS;
}