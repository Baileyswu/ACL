#include "nfa.h"
#include <iostream>
#include <queue>
using namespace std;
using namespace NFA;

Nfa::Nfa(){
	Node nd;
	nodes.push_back(nd);
}

Nfa::~Nfa(){
	nodes.clear();
}

int Nfa::newnode(int parent, char c, bool cycle) {
	Node nd(cycle);
	int id = nodes.size();
	nodes.push_back(nd);
	nodes[parent].mp[c] = id;
	return id;
}

int NFA::Nfa::push_to_queue(std::deque<int>& Q, std::vector<int>& vis, int toq)
{
	if (vis[toq])
		return 0;
	Q.push_back(toq);
	cout << " " << toq;
	vis[toq] = true;
	return 1;
}

bool NFA::Nfa::can_trans(int id, char c)
{
	MAP mp = nodes[id].mp;
	if (mp.find(c) == mp.end())
		return false;
	int nt = mp[c];
	return nodes[nt].count > 0;
}

Node* NFA::Nfa::goto_next(int& now, int nt)
{
	now = nt;
	Node* nd = &nodes[now];
	(*nd).count++;
	return nd;
}

int Nfa::insert(const char buf[]){
	int len = strlen(buf);
	int now = 0;
	Node *nd = &nodes[now];
	MAP mp;
	
	if (len == 0) return 0;

	nodes[now].count++;
	for (int i = 0; i < len; i++) {
		mp = (*nd).mp;
		if (mp.find(buf[i]) == mp.end()) {
			if (buf[i] == FREE) {
				mp[buf[i]] = newnode(now, buf[i], true);
				nd = goto_next(now, mp[buf[i]]);
				mp = (*nd).mp;
			}
			mp[buf[i]] = newnode(now, buf[i], false);
		}
		else {
			if (buf[i] == FREE) {
				nd = goto_next(now, mp[buf[i]]);
				mp = (*nd).mp;
			}
		}
		nd = goto_next(now, mp[buf[i]]);
	}
	(*nd).is_end = true;
	return 1;
}

int Nfa::del(const char buf[]){
	return 0;
}

int Nfa::query(const char buf[]){
	show();
	cout << buf << endl;
	int len = strlen(buf);
	int now = 0;
	std::deque<int> Q;
	std::vector<int> vis(sizeof(nodes));
	// clear
	push_to_queue(Q, vis, now);
	for (int i = 0; i < len; i++) {
		cout << endl << buf[i] << ": ";
		int size = Q.size();
		if (size == 0) return 0;
		vis.assign(nodes.size(), false);
		while (size--) {
			int fd = Q.front(); 
			MAP mp = nodes[fd].mp;

			Q.pop_front(); 

			if (can_trans(fd, FREE) && nodes[mp[FREE]].is_cycle) {
				push_to_queue(Q, vis, mp[FREE]);
				push_to_queue(Q, vis, nodes[mp[FREE]].mp[FREE]);
			}
			if (nodes[fd].is_cycle) {
				push_to_queue(Q, vis, fd);
				push_to_queue(Q, vis, mp[FREE]);
			}
			if(can_trans(fd, buf[i])) {
				push_to_queue(Q, vis, mp[buf[i]]);
			}
		}
	}
	while (!Q.empty()) {
		int fd = Q.front();
		Q.pop_front();
		if (nodes[fd].is_end)
			return 1;
	}
	return 0;
}

int NFA::Nfa::show()
{
	cout << "\n==================\n";
	for (int i = 0; i < nodes.size(); i++) {
		Node nd = nodes[i];
		cout << "node " << i << " " << nodes[i].count 
			<< (nodes[i].is_cycle ? " Is_cycle " : " ")
			<< (nodes[i].is_end ? " Is_end " : " ") << endl;
		for (MAP::iterator it = nd.mp.begin(); it != nd.mp.end(); it++)
			cout << " <" << it->first << ", " << it->second << ">" << endl;
	}
	return 0;
}

