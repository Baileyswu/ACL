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

#define FILE_NAME "blocklist.txt"
#define READ_ERR -80
using namespace std;
using namespace XSOCKET;

XSocket skt;
int send_time;

pthread_mutex_t write_time_mutex = PTHREAD_MUTEX_INITIALIZER;

void Print_Command() {
	printf("=================================\n"\
		"Input format: <command> <string>  \n"\
		"Command List:\n"\
		"  a: insert rule\n"\
		"  d: delete rule\n"\
		"  q: query string\n"\
		"Example: \n"\
		"  a a*b\n");
}

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
	if (command != 'a' && command != 'q' && command != 'd') {
		Print_Command();
		return Read_From_Keyboard(command, str);
	}
	int len = strlen(str);
	if (len < 1) {
		Print_Command();
		return Read_From_Keyboard(command, str);
	}
	for (int i = 0; i < len; i++)
		if (str[i] == '*' && i + 1 < len && str[i + 1] == '*') {
			Print_Command();
			return Read_From_Keyboard(command, str);
		}
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
	int retcode;

	send_time = CurrentTime();
	FILE* pFile;
	pFile = fopen(FILE_NAME, "r");
	if (pFile == NULL) {
		perror("Error opening file");
		printf("Add file %s in your folder\n", FILE_NAME);
		Sleep(3000);
		pthread_exit(0);
		return nullptr;
	}

	while (Read_From_File(pFile, command, str) == SUCCESS 
		|| Read_From_Keyboard(command, str) == SUCCESS) {
		Pack_Msg(command, str, send_buf, len);
		if ((retcode = skt.Send(send_buf, len)) == SUCCESS 
			&& (retcode = skt.Receive(recv_buf, len)) == SUCCESS) {
			Decode_Msg(recv_buf, len);
			pthread_mutex_lock(&write_time_mutex);
			send_time = CurrentTime();
			pthread_mutex_unlock(&write_time_mutex);
			continue;
		}
		if (retcode == WSAECONNRESET) {
			printf("Can't connect to server\n");
			break;
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
		if (send_time + RECV_CIRCLE_TIME - 1 < CurrentTime()) {
			skt.Make_Heart_Beat(send_buf, len);
			if(skt.Send(send_buf, len) == WSAECONNRESET) {
				printf("Can't connect to server\n");
				break;
			}
			pthread_mutex_lock(&write_time_mutex);
			send_time = CurrentTime();
			pthread_mutex_unlock(&write_time_mutex);
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
	pthread_join(heartbeat_id, &main_id);
	pthread_detach(work_id);

	skt.Close_Client();

	return SUCCESS;
}