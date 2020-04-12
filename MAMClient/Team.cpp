#include "stdafx.h"
#include "Team.h"

#include "Player.h"
#include "Client.h"
#include "pTeam.h"

#include "MessageManager.h"

CTeam *team = nullptr;

CTeam::CTeam() {
	//
}

void CTeam::Create() {
	if (team) {
		messageManager.DoSystemMessage("You are already in a team.");
		return;
	}
	if (!team) team = new CTeam();
	pTeam *msg = new pTeam(TEAM_ACT_CREATE, player->GetID(), 0);
	gClient.addPacket(msg);
	team->AddMember(player);
}

void CTeam::Join(User *user) {
	if (!user) return;
	if (team) {
		messageManager.DoSystemMessage("You are already in a team.");
		return;
	}
	pTeam *msg = new pTeam(TEAM_ACT_JOIN, player->GetID(), user->GetID());
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

void CTeam::AddMember(User *pUser) {
	if (members.size() > 4) {
		messageManager.DoSystemMessage("The team is full.");
		return;
	}
	members.push_back(pUser);
}