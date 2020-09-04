#ifndef __CLIENT_H
#define __CLIENT_H

//For local and new server testing
//#define DEVSERVER
//#define LOCALSERVER
#define DEVPORT

#define DEBUG_LOG
#define DEBUG_PACKET
//#define SIZE_1024
#define NEWGUI

#include "GUI.h"
#include "GameLibrary.h"
#include "WDF.h"
#include "INI.h"

#include "Cipher.h"
#include "Packet.h"

enum wdfType {
	wData,
	wCharacter,
	wPet
};

class pLoginResponse;

class Client {
private:
	SDL_Thread *asyncThread, *recvThreadAcct, *recvThreadGame;
	bool closeThread = false;
	SDL_mutex* sendMutex = NULL;
	SDL_mutex* packetMutex = NULL;

	SDL_mutex *cwdfMutex = NULL, *dwdfMutex = NULL, *pwdfMutex = NULL;
	WDF *wdfData, *wdfCharacter, *wdfPet;

	//For Socket Communication
	std::ofstream debugLog, packetLog;
	bool socketDebug = true;
	SOCKADDR_IN accountAddr, gameAddr;
	SOCKET accountSocket, gameSocket;
	SOCKET* activeSocket = nullptr;
	std::queue<Packet*> sendQueue, receiveQueue;
	Cipher encryptCipher, decryptCipher;
	//Cipher cipher;
	bool useSeedAfterSend = false, useSeed = false;
	bool checkGsConnection = false, socketDisconnected = false;

	static int Client::receivePacket(void *ptr);
	void Client::sendPacket();
	void Client::createPacketByType(int type, int size, char* header, char* buffer);

	static int Client::asyncClient(void *ptr);
	bool Client::loadWDF();
	void Client::loadINI();
	

public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	//GUI* gui = nullptr;
	int windowWidth, windowHeight;

	INI *configIni, *npcRle, *shopIni, *itemIni;

	bool mainReady = false;
	int accountId;
	int battleFormation = 0;

	Client();
	~Client();
	void Client::initializeGUI();
	void Client::shutdownThread();

	void Client::getFileFromWDF(std::string wdfFile, std::shared_ptr<DataBuffer> &buffer, int &size);
	int Client::stringToWdf(std::string wdfFile);

	void Client::encryptPacket(BYTE* encryptedBuffer, BYTE* buffer, int len);
	void Client::decryptPacket(BYTE* decryptedBuffer, BYTE* buffer, int len);
	void Client::addPacket(Packet* packet);
	void Client::addPacket(std::vector<Packet*> packets);
	void Client::logPacketError(std::string error);
	void writePacketToLog(Packet* packet);

	void Client::login(std::string user, std::string password, std::string server);
	bool Client::connectAccountServer();
	bool Client::connectGameServer();
	void Client::shutdownSocket();
	bool Client::isConnectionAlive();
	bool Client::wasDisconnected();
	
	void Client::handlePackets();
	Packet* Client::getNextPacket();

public: //Login
	void handleLoginResponse(pLoginResponse* packet);
	bool getLoginResponse(int &account, int &seed, std::string &message);
	void clearLoginResponse();
private:
	bool loginResponseReceived = false;
	int loginSeed;
	std::string loginMessage;

public: //Debugging
	SDL_mutex *dbgMutex = NULL;
	void addToDebugLog(std::string message);
};

extern Client gClient;
extern std::string strServer;

#endif
