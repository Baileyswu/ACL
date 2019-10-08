#ifndef _LIB_PROTOCAL_H__
#define _LIB_PROTOCAL_H__

#define PROTOCOL_TYPE 'n'
#define HEART_BEAT 'h'
#define BODY_SIZE 256

struct N_HEADER
{
	char protocol;
	char version;
	char pkgFlag;
	char command;
	int datalen;
};

struct N_MESSAGE
{
	N_HEADER header;
	char body[BODY_SIZE];

	int getsize();
};

int N_MESSAGE::getsize() {
	return header.datalen + sizeof(N_HEADER);
}


N_MESSAGE* Make_Message(char a, char b, char c, char* d) {
	N_MESSAGE* msg = new N_MESSAGE;

	if (msg != nullptr) {
		msg->header.protocol = a;
		msg->header.version = '1';
		msg->header.pkgFlag = b;
		msg->header.command = c;
		msg->header.datalen = strlen(d);
		memcpy(msg->body, d, sizeof(d));
		msg->body[strlen(d)] = '\0';
	}

	return msg;
}

#endif // !__LIB_PROTOCAL_H__
