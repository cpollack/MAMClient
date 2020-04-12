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
	void AddMember(User *pUser);
private:
	std::vector<User*> members;
};

extern CTeam *team;