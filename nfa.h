#include <map>
#include <vector>
#include <queue>

using namespace std;

namespace NFA
{
	const char FREE = '*';
	typedef std::map<char, int> MAP;

	struct Node
	{
		Node() : is_end(false), is_cycle(false), count(0) {}
		Node(int is_cycle): is_end(false), is_cycle(is_cycle), count(0) {}
		bool is_end;
		bool is_cycle;
		int count;
		MAP mp;
	};

	class Nfa
	{
		std::vector<Node> nodes;
		int newnode(int parent, char c, bool cycle);
		int push_to_queue(std::deque<int>& Q, std::vector<int> &vis, int toq);
		bool can_trans(int id, char c);
		Node* goto_next(int& now, int nt);
	public:
		Nfa();
		~Nfa();
		int insert(const char buf[]);
		int del(const char buf[]);
		int query(const char buf[]);
		int show();
	};

}