#include "XSocket.h"
#include "Protocal.h"
#include <iostream>
#include <windows.h>
#include <time.h>

#define FILE_NAME "input.txt"
#define READ_ERR -80
using namespace std;
using namespace XSOCKET;

XSocket skt;

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
	case 'y': outprint = "success"; break;
	case 'n': outprint = "failed"; break;
	default: outprint = "unknown";
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
		command = str[0];
		find = strchr(str, '\n');
		if (find)
			*find = '\0';
		return SUCCESS;
	}
	return READ_ERR;
}

void Work_Thread() 
{
	printf("%s line:%d\n", __func__, __LINE__);
	char command;
	char str[BUFSIZ];
	char send_buf[BUFSIZ];
	char recv_buf[BUFSIZ];
	int len;
	int send_time = CurrentTime();
	FILE* pFile;
	pFile = fopen(FILE_NAME, "r");
	if (pFile == NULL) {
		perror("Error opening file");
		return;
	}

	while (Read_From_File(pFile, command, str) == SUCCESS) {
		Pack_Msg(command, str + 2, send_buf, len);
		if (skt.Send(send_buf, len) == SUCCESS && skt.Receive(recv_buf, len) == SUCCESS) {
			Decode_Msg(recv_buf, len);
			send_time = CurrentTime();
			Sleep(3000);
			continue;
		}
		if (send_time + RECV_CIRCLE_TIME < CurrentTime()) {
			skt.Make_Heart_Beat(send_buf, len);
			skt.Send(send_buf, len);
		}
	}

	fclose(pFile);
}

int main()
{
	printf("%s line:%d\n", __func__, __LINE__);

	if (skt.Start_Client() != SUCCESS) {
		skt.Close_Client();
		return XSOCKET_ERROR;
	}

	Work_Thread();

	skt.Close_Client();

	return SUCCESS;
}