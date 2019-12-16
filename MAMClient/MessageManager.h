#pragma once

struct SMessage {
	std::string sender;
	std::string target;
	std::string message;
	SDL_Color color;
	std::string emotion;
	int channel;
	int effect;
};

class pMessage;

using Message = std::shared_ptr<SMessage>;

class MessageManager {
public:
	MessageManager();
	~MessageManager();

	Message Poll();
	void Push(SMessage msg);
	void Push(pMessage* pMsg);

private:
	std::queue<Message> messages;
};

extern MessageManager messageManager;