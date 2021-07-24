#pragma once


class User;
class CTeam {
public:
	CTeam();
	~CTeam() {}

	static void Create();
	static void Join(User *user);
	static void Leave();

	void SetFormation();
	void AllowJoin();
	void BlockJoin();
	void Kick(User *user);

	User* GetLeader();
	User* GetMember(int position);
	User* GetNextInLine(User *user);
	int GetMemberCount();
	void AddMember(User *pUser);
	void RemoveMember(User *pUser);
private:
	std::vector<User*> members;
};