#include "stdafx.h"
#include "MessageManager.h"

#include "pMessage.h"

MessageManager::MessageManager() {
	//
}

MessageManager::~MessageManager() {
	//
}

Message MessageManager::Poll() {
	if (messages.size()) {
		Message poll = messages.front();
		messages.pop();
		return poll;
	}
	return Message(nullptr);
}

void MessageManager::Push(SMessage msg) {
	Message message(new SMessage);
	message->sender = msg.sender;
	message->target = msg.target;
	message->emotion = msg.emotion;
	message->message = msg.message;
	message->channel = msg.channel;
	message->effect = msg.effect;
	message->color = msg.color;
	messages.push(message);
}

void MessageManager::Push(pMessage* pMsg) {
	SMessage message;
	message.sender = pMsg->sender;
	message.target = pMsg->target;
	message.emotion = pMsg->emotion;
	message.message = pMsg->message;
	message.channel = pMsg->channel;
	message.effect = pMsg->effect;
	message.color = { (Uint8)((pMsg->color & 0xFF0000) >> 16), (Uint8)((pMsg->color & 0xFF00) >> 8), (Uint8)(pMsg->color & 0xFF), (Uint8)0x255 };
	Push(message);
}