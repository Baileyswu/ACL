#include "nfa.h"
#include <assert.h>
#include <iostream>
using namespace std;
using namespace NFA;

void Print_Msg(char* a, int len) {
	putchar('\n');
	for (int i = 0; i < len; i++)
		putchar(a[i]);
	putchar('\n');
}

int case1() {
	Nfa g;
	assert(g.Insert_Rule("a*b") == NFA_SUCCESS);
	assert(g.Insert_Rule("abcd") == NFA_SUCCESS);
	assert(g.Insert_Rule("") == NFA_ERR);

	assert(g.Query("abbcd") == NFA_ERR);
	assert(g.Query("abcd") == NFA_SUCCESS);
	assert(g.Query("abcde") == NFA_ERR);
	assert(g.Query("abbcb") == NFA_SUCCESS);
	assert(g.Query("ab") == NFA_SUCCESS);

	assert(g.Delete_Rule("a*b") == NFA_SUCCESS);

	assert(g.Query("ab") == NFA_ERR);
	assert(g.Query("abbcb") == NFA_ERR);
	assert(g.Query("abcd") == NFA_SUCCESS);
	return 0;
}

int case2(){
	Nfa n;
	assert(n.Insert_Rule("www.*.com") == NFA_SUCCESS);
	assert(n.Query("www.baidu.com") == NFA_SUCCESS);
	return 0;
}

int case3() {
	Nfa g;
	char str[100] = "n1na3000a*b";
	char back[100];
	int len = 11;
	assert(g.Parse_Data(str, back, len) == NFA_SUCCESS);
	Print_Msg(back, len);
	assert(back[len - 1] == 'y');

	strcpy(str, "n1na4000abcd");
	len = 12;
	assert(g.Parse_Data(str, back, len) == NFA_SUCCESS);
	Print_Msg(back, len);
	assert(back[len - 1] == 'y');

	strcpy(str, "n1na3000a*c");
	len = 11;
	assert(g.Parse_Data(str, back, len) == NFA_SUCCESS);
	Print_Msg(back, len);
	assert(back[len - 1] == 'y');

	strcpy(str, "nlna0000");
	len = 8;
	assert(g.Parse_Data(str, back, len) == NFA_SUCCESS);
	Print_Msg(back, len);
	assert(back[len - 1] == 'n');

	strcpy(str, "nlnq5000abbcd");
	len = 13;
	assert(g.Parse_Data(str, back, len) == NFA_SUCCESS);
	Print_Msg(back, len);
	assert(back[len - 1] == 'n');

	strcpy(str, "nlnq4000abcd");
	len = 12;
	assert(g.Parse_Data(str, back, len) == NFA_SUCCESS);
	Print_Msg(back, len);
	assert(back[len - 1] == 'y');

	return 0;
}

int case4() {
	Nfa g;
	assert(g.Insert_Rule("abcd") == NFA_SUCCESS);
	assert(g.Insert_Rule("a") == NFA_SUCCESS);
	assert(g.Insert_Rule("a") == NFA_REPEAT);
	assert(g.Query("abcd") == NFA_SUCCESS);
	assert(g.Delete_Rule("a") == NFA_SUCCESS);
	assert(g.Query("a") == NFA_ERR);
	assert(g.Query("abcd") == NFA_SUCCESS);
	return 0;
}

int main() {
	case1();
	case4();
	cout << "\n============\nTest OK!\n============\n" << endl;
}