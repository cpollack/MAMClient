#ifndef __PLOGINREQUEST_H
#define __PLOGINREQUEST_H

#include "Packet.h"

class pLoginRequest : public Packet {
private:
	std::string user, pass, serv;

public:
	pLoginRequest(char* userName, char* password, char* server);
	~pLoginRequest();

	void pLoginRequest::debugPrint();
};

#endif