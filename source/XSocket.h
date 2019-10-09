#ifndef _LIB_XSOCKET__
#define _LIB_XSOCKET__

#pragma once
#include <WinSock2.h>

#define RECV_CIRCLE_TIME 5
#define TIME_OUT_LIMIT 30


namespace XSOCKET
{
	const char PROTOCOL_TYPE = 'n';
	const char HEART_BEAT = 'h';
	enum RETCODE
	{
		HEARTBEAT = -46,
		XSOCKET_ERROR = -45,
		XSOCKET_CLOSED = -44,
		NOT_RIGHT_PROTOCAL = -43,
		PKG_TOO_LONG = -42,
		SUCCESS = 0,
	};

	class XSocket
	{
	private:
		sockaddr_in _sockAddr;
		SOCKET _servSock;
		SOCKET _clientSock;
		int _nSize;
		WSADATA _wsaData;

	public:
		XSocket();
		XSocket(const char* ip, const int port);
		int Get_Error_Code(int& sockfd);
		void Print_Func(const char* func, int line);
		void Print_Info(const char* str, int err_code);
		void Print_Buffer(char* buf, int len);
		int Start_Server();
		int Close_Server();
		int Close_Client(int& clntSock);
		int Start_Client();
		int Close_Client();
		int Accept(int& clntSock, SOCKADDR& clntAddr);
		int Receive(int& clntSock, char* buffer, int& slen);
		int Send(int& clntSock, char* cstr, int& len);
		int Send(char* cstr, int& len);
		int Receive(char* buffer, int& slen);
		int Make_Heart_Beat(char* cstr, int& len);
	};

} // namespace XSOCKET

#endif