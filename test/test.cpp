#include "nfa.h"
#include <assert.h>
#include <iostream>
using namespace std;
using namespace NFA;


int main(){
	Nfa g;
	assert(g.insert("a*b") == 0);
	assert(g.insert("abcd") == 0);
	assert(g.insert("a*c") == 0);
	assert(g.insert("") == 1);

	assert(g.query("abbcd") == 1);
	assert(g.query("abcd") == 0);
	assert(g.query("abcde") == 1);
	assert(g.query("abbcb") == 0);

	assert(g.del("a*b") == 0);

	assert(g.query("ab") == 1);
	assert(g.query("abbcb") == 1);
	assert(g.query("abcd") == 0);

	Nfa n;
	assert(n.insert("www.*.com") == 0);
	assert(n.query("www.baidu.com") == 0);

	cout << "\n============\nTest OK!\n============\n" << endl;

	return 0;
}