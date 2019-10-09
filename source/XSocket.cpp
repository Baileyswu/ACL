#include "XSocket.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#include <stdio.h>

using namespace XSOCKET;

XSocket::XSocket()
{
	*this = XSocket("127.0.0.1", 25525);
}

XSocket::XSocket(const char* ip, const int port)
{
	memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr.s_addr = inet_addr(ip);
	_sockAddr.sin_port = htons(port);
	_nSize = sizeof(SOCKADDR);
}

int XSocket::Start_Server()
{
	int err_code;
	int iResult;
	int intServSock;
	int on = 1;
#ifdef _WIN32
	iResult = WSAStartup(MAKEWORD(2, 2), &_wsaData);
	if (iResult != NO_ERROR) {
		Print_Info("WSAStartup failed with error", iResult);
		return iResult;
	}
#endif
	_servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	intServSock = _servSock;
	if (_servSock == INVALID_SOCKET) {
		err_code = Get_Error_Code(intServSock);
		Print_Info("socket failed with error", err_code);
		return err_code;
	}


#ifdef _WIN32
	iResult = setsockopt(_servSock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
#else
	iResult = setsockopt(_servSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif
	if (iResult == SOCKET_ERROR) {
		err_code = Get_Error_Code(intServSock);
		Print_Info("setsockopt SO_REUSEADDR failed", err_code);
		return err_code;
	}

	iResult = bind(_servSock, (SOCKADDR*)&_sockAddr, sizeof(SOCKADDR));
	if (iResult == SOCKET_ERROR) {
		err_code = Get_Error_Code(intServSock);
		Print_Info("bind failed with error", err_code);
		return err_code;
	}

	iResult = listen(_servSock, SOMAXCONN);
	printf("start ret = %u\n", iResult);
	if (iResult == SOCKET_ERROR) {
		err_code = Get_Error_Code(intServSock);
		Print_Info("listen failed with error", err_code);
		return err_code;
	}
	printf("\nServer %u starts.\n", _servSock);
	return SUCCESS;
}

int XSocket::Close_Server()
{
	Print_Func(__func__, __LINE__);
#ifdef _WIN32
	closesocket(_servSock);
	WSACleanup();
#else
	close(_servSock);
#endif
	return SUCCESS;
}

int XSocket::Close_Client(int& clntSock)
{
	Print_Func(__func__, __LINE__);
#ifdef _WIN32
	closesocket(clntSock);
#else
	close(clntSock);
#endif
	printf("\nClient to %u closed.\n", clntSock);
	return SUCCESS;
}

int XSocket::Start_Client()
{
	int err_code;
	int iResult;
	int intServSock;
	int on = 1;
#ifdef _WIN32
	iResult = WSAStartup(MAKEWORD(2, 2), &_wsaData);
	if (iResult != NO_ERROR) {
		Print_Info("WSAStartup failed with error", iResult);
		return iResult;
	}
#endif
	_clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	intServSock = _clientSock;
	if (_servSock == INVALID_SOCKET) {
		err_code = Get_Error_Code(intServSock);
		Print_Info("socket failed with error", err_code);
		return err_code;
	}

	if (connect(_clientSock, (struct sockaddr*)&_sockAddr, sizeof(_sockAddr))) {
		printf("Client Connect Port Failed!\n");
		return XSOCKET_ERROR;
	}

	return SUCCESS;
}

int XSocket::Close_Client()
{
	Print_Func(__func__, __LINE__);
#ifdef _WIN32
	closesocket(_clientSock);
	WSACleanup();
#else
	close(_clientSock);
#endif
	return SUCCESS;
}

int XSocket::Accept(int& clntSock, SOCKADDR& clntAddr)
{
	Print_Func(__func__, __LINE__);
	int iResult;
	int err_code;
	iResult = accept(_servSock, (SOCKADDR*)&clntAddr, &_nSize);
	if (iResult == INVALID_SOCKET) {
		err_code = Get_Error_Code(clntSock);
		Print_Info("accept failed with error", err_code);
		return err_code;
	}
	else {
		clntSock = iResult;
		printf("\nClient to %u connected.\n", clntSock);
	}

#ifdef _WIN32
	int time_out = RECV_CIRCLE_TIME * 1000;
	iResult = setsockopt(clntSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&time_out, sizeof(time_out));
#else
	struct timeval time_out = { RECV_CIRCLE_TIME, 0 };
	iResult = setsockopt(clntSock, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out));
#endif

	if (iResult == SOCKET_ERROR) {
		err_code = Get_Error_Code(clntSock);
		Print_Info("setsockopt for SO_RCVTIMEO failed with error", err_code);
		return err_code;
	}
	else
		printf("SO_RCVTIMEO Value: %d\n", RECV_CIRCLE_TIME);
	return SUCCESS;
}

int XSocket::Receive(int& clntSock, char* buffer, int& slen) 
{
	Print_Func(__func__, __LINE__);
	int err_code;
	int pkg_len;
	int iResult;

	memset(buffer, 0, BUFSIZ);
	slen = recv(clntSock, buffer, BUFSIZ, 0);
	if (slen > 0)
		printf("Bytes received: %d\n", slen);
	else if (slen == 0) {
		printf("Connection closed\n");
		return XSOCKET_CLOSED;
	}
	else {
		err_code = Get_Error_Code(clntSock);
		printf("recv failed: %d\n", err_code);
		return err_code;
	}

	if (slen > BUFSIZ) {
		printf("slen is too large (slen > BUFSIZ)\n");
		return XSOCKET_ERROR;
	}

	if (buffer[0] != PROTOCOL_TYPE) {
		printf("Error: not XSTEP protocal\n");
		return NOT_RIGHT_PROTOCAL;
	}

	if (buffer[2] == HEART_BEAT) {
		printf("HEARTBEAT pkgFlag = 'h'\n");
		return HEARTBEAT;
	}

	buffer[slen] = '\0';
	return SUCCESS;
}

int XSocket::Send(int& clntSock, char* cstr, int& len) 
{
	Print_Func(__func__, __LINE__);
	int iResult;
	int err_code;
	cstr[len] = '\0';
	// Print_Buffer(cstr, len);
	iResult = send(clntSock, cstr, len, 0);
	if (iResult == SOCKET_ERROR) {
		err_code = Get_Error_Code(clntSock);
		Print_Info("send failed with error", err_code);
		return err_code;
	}
	return SUCCESS;
}

int XSocket::Send(char* cstr, int& len) 
{
	Print_Func(__func__, __LINE__);
	int clntSock = _clientSock;
	return Send(clntSock, cstr, len);
}

int XSocket::Receive(char* buffer, int& slen) 
{
	Print_Func(__func__, __LINE__);
	int clntSock = _clientSock;
	int retcode;
	while ((retcode = Receive(clntSock, buffer, slen)) == HEARTBEAT);
	return retcode;
}


int XSocket::Make_Heart_Beat(char* cstr, int& len)
{
	char ret[10] = "n1h00000\0";
	len = 8;
	memcpy(cstr, ret, len);
	return SUCCESS;
}

int XSocket::Get_Error_Code(int& sockfd)
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	// int len;
	// int err = -1;
	// getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, (socklen_t *)&len);
	if (errno != 0)
	{
		fputs(strerror(errno), stderr);
		fputs(": ", stderr);
	}
	return errno;
#endif
}

void XSocket::Print_Func(const char* func, int line)
{
#if DEBUG
#ifdef _WIN32
	printf("%s line:%d\n", func, line);
#else
	printf("\033[1;34m%s line:%d\033[0m\n", func, line);
#endif
#endif
}

void XSocket::Print_Info(const char* str, int err_code)
{
#ifdef _WIN32
	wprintf(L"%ls: %ld\n", str, err_code);
#else
	printf("%s: %d\n", str, err_code);
#endif
}

void XSocket::Print_Buffer(char* buf, int len)
{
	printf("len = %d \n head:", len);
	for (int i = 0; i < 8; i++)
		printf("%02hd ", buf[i]);
	printf("\n");
	for (int i = 8; i < len; i++)
		printf("%c", buf[i]);
	printf("\n");
}