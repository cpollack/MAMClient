#include "stdafx.h"
#include "pTeamMember.h"

#include "UserManager.h"
#include "Player.h"
#include "Team.h"

pTeamMember::pTeamMember(char* buf, char* encBuf) {
	description = "Team Member (Server)";
	type = ptTeamMember;
	initBuffer(28);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(4, &memberId[0]);
	getInt(8, &memberId[1]);
	getInt(12, &memberId[2]);
	getInt(16, &memberId[3]);
	getInt(20, &memberId[4]);
}


pTeamMember::~pTeamMember() {
	//
}


void pTeamMember::process() {
	User* userLeader = userManager.getUserById(memberId[0]);
	if (!userLeader) return;

	userLeader->CreateTeam();
	CTeam *pTeam = userLeader->GetTeam();

	assert(pTeam);
	
	for (int i = 1; i < 5; i++) {
		if (memberId[i] == 0) break;
		User* userMember = userManager.getUserById(memberId[i]);
		if (userMember) {
			userMember->JoinTeam(pTeam);
		}
	}
}


void pTeamMember::debugPrint() {
	Packet::debugPrint();

	std::cout << "Member1:" << memberId[0] << " Member2:" << memberId[1] << " Member3:" << memberId[2] << " Member4:" << memberId[3] << " Member5:" << memberId[4] << std::endl;

	std::cout << std::endl;
}