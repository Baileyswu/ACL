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

Node* NFA::Nfa::goto_next(int& now, int nt, bool addcount)
{
	now = nt;
	Node* nd = &nodes[now];
	if(addcount)
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
			bool is_cycle = buf[i] == FREE ? true : false;
			mp[buf[i]] = newnode(now, buf[i], is_cycle);
		}
		nd = goto_next(now, mp[buf[i]], true);
	}
	(*nd).is_end = true;
	return 1;
}

int Nfa::del(const char buf[]){
	int len = strlen(buf);
	std::vector<int> path;
	int now = 0;
	Node* nd = &nodes[now];
	MAP mp;

	if (len == 0)
		return 1;

	int maxflow = nodes[now].count;
	path.push_back(now);
	for (int i = 0; i < len; i++) {
		mp = (*nd).mp;
		if (mp.find(buf[i]) == mp.end())
			return 1;
		nd = goto_next(now, mp[buf[i]], false);
		maxflow = min(maxflow, (*nd).count);
		path.push_back(mp[buf[i]]);
;	}

	for (int i = 0; i < path.size(); i++) {
		int id = path[i];
		nodes[id].count -= maxflow;
		if (nodes[id].count == 0) {
			nodes[id].is_end = nodes[id].is_cycle = false;
		}
	}
	show();

	return 0;
}

int Nfa::query(const char buf[]){
	show();
	cout << "------->" << buf << endl;
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

			if (can_trans(fd, FREE)) {
				push_to_queue(Q, vis, mp[FREE]);
			}
			if (nodes[fd].is_cycle) {
				push_to_queue(Q, vis, fd);
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

