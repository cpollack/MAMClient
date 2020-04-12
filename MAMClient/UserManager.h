#pragma once

class User;

class UserManager {
public:
	UserManager();
	~UserManager();

	void addUser(User *pUser);
	User* getUserById(int id);
	std::vector<User*> getUsers();
	void clear();

	//Batch Handling for all users
	void stepAllUsers();
	void handleEventAllUsers(SDL_Event& e);

private:
	std::map<int, User*> Users;
};

extern UserManager userManager;