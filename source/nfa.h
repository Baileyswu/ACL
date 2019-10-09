#ifndef _LIB_NFA__
#define _LIB_NFA__

#include <map>
#include <vector>
#include <queue>
#include <set>
#include <string>

namespace NFA
{
	const char FREE = '*';
	typedef std::map<char, int> MAP;

	enum RETCODE
	{
		NFA_SUCCESS = 0,
		NFA_ERR = 1,
		NFA_REPEAT = 2,
	};

	struct Node
	{
		Node() : endc(0), is_cycle(false), count(0) {}
		Node(int is_cycle): endc(0), is_cycle(is_cycle), count(0) {}
		int endc;
		bool is_cycle;
		int count;
		MAP mp;
	};

	class Nfa
	{
		std::vector<Node> nodes;
		std::set<std::string> rules;
		int newnode(int parent, char c, bool cycle);
		int push_to_queue(std::deque<int>& Q, std::vector<int> &vis, int toq);
		bool can_trans(int id, char c);
		Node* goto_next(int& now, int nt, bool addcount);
	public:
		Nfa();
		~Nfa();
		int Insert_Rule(const char buf[]);
		int Delete_Rule(const char buf[]);
		int Query(const char buf[]);
		int Show();
		int Opeartion(char c, char* str);
		int Parse_Data(char* recv, char* send, int& len);
	};

}

#endif // !_LIB_NFA__
