#include "stdafx.h"
#include "Team.h"

#include "Player.h"
#include "Client.h"
#include "pTeam.h"

#include "MessageManager.h"

CTeam::CTeam() {
	//
}

void CTeam::Create() {
	if (player->GetTeam()) {
		messageManager.DoSystemMessage("You are already in a team.");
		return;
	}
	pTeam *msg = new pTeam(TEAM_ACT_CREATE, player->GetID(), 0);
	gClient.addPacket(msg);
}

void CTeam::Join(User *user) {
	if (!user) return;
	if (player->GetTeam()) {
		messageManager.DoSystemMessage("You are already in a team.");
		return;
	}
	pTeam *msg = new pTeam(TEAM_ACT_JOIN, player->GetID(), user->GetID());
	gClient.addPacket(msg);
}

void CTeam::Leave() {
	if (!player->GetTeam()) {
		messageManager.DoSystemMessage("You are not in a team.");
		return;
	}

	pTeam *msg = new pTeam(TEAM_ACT_QUIT, player->GetID(), 0);
	gClient.addPacket(msg);
}

User* CTeam::GetLeader() {
	return GetMember(1);
}

User* CTeam::GetMember(int position) {
	if (position > members.size()) return nullptr;
	if (position < 1) return nullptr;
	return members[position - 1];
}

User* CTeam::GetNextInLine(User *user) {
	if (members[0] == user) return nullptr;
	if (members.size() == 1) return nullptr;

	User *nextInLine = members[0];
	for (int i = 1; i < members.size(); i++) {
		if (members[i] == user) return nextInLine;
		nextInLine = members[i];
	}
	return nullptr;
}

int CTeam::GetMemberCount() {
	return members.size();
}

void CTeam::AddMember(User *pUser) {
	if (members.size() > 4) {
		messageManager.DoSystemMessage("The team is full.");
		return;
	}
	members.push_back(pUser);
}

void CTeam::RemoveMember(User *pUser) {
	std::vector<User*>::iterator itr = members.begin();
	while (itr != members.end()) {
		if (*itr == pUser) {
			members.erase(itr);
			break;
		}
		itr++;
	}
}