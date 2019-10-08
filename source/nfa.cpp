#include "nfa.h"
#include "Protocal.h"
#include <iostream>
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
	int id = (int)nodes.size();
	nodes.push_back(nd);
	nodes[parent].mp[c] = id;
	return id;
}

int Nfa::push_to_queue(std::deque<int>& Q, std::vector<int>& vis, int toq)
{
	if (vis[toq])
		return NFA_ERR;
	Q.push_back(toq);
	cout << " " << toq;
	vis[toq] = true;
	return NFA_SUCCESS;
}

bool Nfa::can_trans(int id, char c)
{
	MAP mp = nodes[id].mp;
	if (mp.find(c) == mp.end())
		return false;
	int nt = mp[c];
	return nodes[nt].count > 0;
}

Node* Nfa::goto_next(int& now, int nt, bool addcount)
{
	now = nt;
	Node* nd = &nodes[now];
	if(addcount)
		(*nd).count++;
	return nd;
}

int Nfa::Insert_Rule(const char buf[]){
	size_t len = strlen(buf);
	int now = 0;
	Node *nd = &nodes[now];
	MAP mp;
	
	if (len == 0) {
		printf("No rules to insert");
		return NFA_ERR;
	}

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
	return NFA_SUCCESS;
}

int Nfa::Delete_Rule(const char buf[]){
	size_t len = strlen(buf);
	std::vector<int> path;
	int now = 0;
	Node* nd = &nodes[now];
	MAP mp;

	if (len == 0)
		return NFA_ERR;

	int maxflow = nodes[now].count;
	path.push_back(now);
	for (int i = 0; i < len; i++) {
		mp = (*nd).mp;
		if (mp.find(buf[i]) == mp.end())
			return NFA_ERR;
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
	Show();

	return NFA_SUCCESS;
}

int Nfa::Query(const char buf[]){
	Show();
	cout << "------->" << buf << endl;
	size_t len = strlen(buf);
	int now = 0;
	std::deque<int> Q;
	std::vector<int> vis(sizeof(nodes));
	// clear
	push_to_queue(Q, vis, now);
	for (int i = 0; i < len; i++) {
		cout << endl << buf[i] << ": ";
		size_t size = Q.size();
		if (size == 0) return NFA_ERR;
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
			return NFA_SUCCESS;
	}
	return NFA_ERR;
}

int Nfa::Show()
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
	return NFA_SUCCESS;
}

int Nfa::Opeartion(char c, char* str) {
	switch (c) {
	case 'q':
		return Query(str);
	case 'a':
		return Insert_Rule(str);
	case 'd':
		return Delete_Rule(str);
	}
	return NFA_ERR;
}

int Nfa::Parse_Data(char* recv, char* send, int& len) {
	N_MESSAGE* msg = (N_MESSAGE*)recv;
	N_HEADER* head_ptr = &(msg->header);
	char* str = recv + sizeof(N_HEADER);
	
	if (head_ptr->protocol != PROTOCOL_TYPE)
		return NFA_ERR;

	if (head_ptr->pkgFlag == HEART_BEAT)
		return NFA_ERR;

	int ans = Opeartion(head_ptr->command, str);
	char body[8];
	body[0] = ans == 0 ? 'y' : 'n';
	body[1] = '\0';

	N_MESSAGE* ret = Make_Message(PROTOCOL_TYPE, HEART_BEAT + 1, head_ptr->command, body);

	len = ret->getsize();
	memcpy(send, ret, len);
	return NFA_SUCCESS;
}

