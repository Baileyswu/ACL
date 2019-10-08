#include "nfa.h"
#include <assert.h>
using namespace std;
using namespace NFA;


int main(){
	Nfa g;
	assert(g.insert("a*b") == 1);
	assert(g.insert("abcd") == 1);
	assert(g.insert("a*c") == 1);
	assert(g.insert("") == 0);

	assert(g.query("abbcd") == 0);
	assert(g.query("abcd") == 1);
	assert(g.query("abcde") == 0);
	assert(g.query("abbcb") == 1);

	assert(g.del("a*b") == 1);

	assert(g.query("ab") == 0);
	assert(g.query("abbcb") == 0);
	assert(g.query("abcd") == 1);

	return 0;
}