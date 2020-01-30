#include "stdafx.h"
#include "UserManager.h"

#include "User.h"

UserManager::UserManager() {
	//
}

UserManager::~UserManager() {
	//
}

void UserManager::addUser(User* pUser) {
	Users[pUser->GetID()] = pUser;
}

User* UserManager::getUserById(int id) {
	for (auto user : Users) {
		if (user.second->GetID() == id) return user.second;
	}
	return nullptr;
}

std::vector<User*> UserManager::getUsers() {
	std::vector<User*> userVector;
	for (auto user : Users) userVector.push_back(user.second);
	return userVector;
}

void UserManager::clear() {
	for (auto User : Users) {
		delete User.second;
	}
	Users.clear();
}

void UserManager::stepAllUsers() {
	for (auto it = Users.begin(), next_it = it; it != Users.end(); it = next_it)
	{
		it->second->step();
		++next_it;
		if (it->second->leaveMap())
		{
			delete it->second;
			Users.erase(it);
		}
	}
}