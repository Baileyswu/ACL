#include "XSocket.h"
#include "Protocal.h"
#include <iostream>
#include<windows.h>

using namespace std;
using namespace XSOCKET;

XSocket skt;

int main()
{
	printf("%s line:%d\n", __func__, __LINE__);

	if (skt.Start_Client() != SUCCESS) {
		skt.Close_Client();
		return XSOCKET_ERROR;
	}

	Sleep(10000);

	skt.Close_Client();

	return SUCCESS;
}