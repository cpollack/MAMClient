#include "stdafx.h"
#include "Client.h"
#include "AssetManager.h"
#include "Player.h"
#include "GameMap.h"
#include "Inventory.h"

#include "GameLibrary.h"

#include "pLoginRequest.h"
#include "pLoginResponse.h"
#include "pRole.h"
#include "pGSConnect.h"
#include "pPlayerInfo.h"
#include "pItem.h"
#include "pItemAction.h"
#include "pMapInfo.h"
#include "pPetInfo.h"
#include "pPetAction.h"
#include "pPetShop.h"
#include "pTick.h"
#include "pWalk.h"
#include "pAction.h"
#include "pDirection.h"
#include "pColor.h"
#include "pColosseum.h"
#include "pMessage.h"
#include "pFriend.h"
#include "pUserInfo.h"
#include "pAction.h"
#include "pRename.h"
#include "pUserAttribute.h"
#include "pUserLevelUp.h"
#include "pPetLevelUp.h"

#include "pNpcInfo.h"
#include "pNpc.h"
#include "pNpcDialogue.h"
#include "pNpcAction.h"

#include "pBattleState.h"
#include "pEnemyInfo.h"
#include "pFighterInfo.h"
#include "pBattleAction.h"
#include "pNormalAct.h"
#include "pItemAct.h"
#include "pBattleRound.h"
#include "pBattleResult.h"

std::string strServer = "";

Client::Client() {
	debugLog.open("DebugLog.txt");

	if (!fileExist("data.wdf")) {
		showErrorMessage("Data.wdf is missing. The application will now shutdown.");
		applicationClose();
		return;
	}
	if (!fileExist("character.wdf")) {
		showErrorMessage("Character.wdf is missing. The application will now shutdown.");
		applicationClose();
		return;
	}
	if (!fileExist("pet.wdf")) {
		showErrorMessage("Pet.wdf is missing. The application will now shutdown.");
		applicationClose();
		return;
	}
	if (!fileExist("ini\\GameMap.ini")) {
		showErrorMessage("INI file GameMap is missing. The application will now shutdown.");
		applicationClose();
		return;
	}

	asyncThread = SDL_CreateThread(asyncClient, "AsyncClient", (void*)this);
	if (NULL == asyncThread) {
		std::cout << "SDL_CreateThread failed: " << SDL_GetError() << "\n";
	}

	sendMutex = SDL_CreateMutex();
	packetMutex = SDL_CreateMutex();

	cwdfMutex = SDL_CreateMutex();
	pwdfMutex = SDL_CreateMutex();
	dwdfMutex = SDL_CreateMutex();

	dbgMutex = SDL_CreateMutex();

	wdfData = nullptr;
	wdfCharacter = nullptr;
	wdfPet = nullptr;

	npcRle = nullptr;
	shopIni = nullptr;
	itemIni = nullptr;
}


Client::~Client() {
	closeThread = true;
	SDL_DestroyMutex(sendMutex);
	SDL_DestroyMutex(packetMutex);

	SDL_DestroyMutex(cwdfMutex);
	SDL_DestroyMutex(pwdfMutex);
	SDL_DestroyMutex(dwdfMutex);

	delete gui; gui = nullptr;
	delete wdfData;
	delete wdfCharacter;
	delete wdfPet;
	delete npcRle;
	delete shopIni;
	delete itemIni;

	shutdownSocket();
	debugLog.close();
	packetLog.close();
}


void Client::shutdownThread() {
	closeThread = true;
}


bool Client::connectAccountServer() {
	bool success = true;
	encryptCipher.reset();
	decryptCipher.reset();
	useSeed = false;
	useSeedAfterSend = false;
	checkGsConnection = false;

	//load host dynamically from config
#ifndef LOCALSERVER
#ifndef DEVSERVER
	const char* host = "209.159.153.58";
#else
	const char* host = "127.0.0.1";
#endif // !DEVSERVER
#else
	const char* host = "127.0.0.1";
#endif // !LOCALSERVER


	WSADATA wsa;
	std::cout << "Initializing Winsock." << std::endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "Winsock Initialization failed: " << WSAGetLastError() << std::endl;
		success = false;
		return success;
	}
	
	if ((accountSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		std::cout << "Could not create socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		success = false;
		return success;
	}

	unsigned int accountPort = 9958;

	accountAddr.sin_family = AF_INET;
	accountAddr.sin_port = htons(accountPort);
	accountAddr.sin_addr.s_addr = inet_addr(host);

	// Make a connection to the server with socket SendingSocket.
	int retCode;
	retCode = connect(accountSocket, (SOCKADDR *)&accountAddr, sizeof(accountAddr));
	if (retCode != 0) {
		printf("Client: connect() failed! Error code: %ld\n", WSAGetLastError());
		// Close the socket
		closesocket(accountSocket);
		// Do the clean up
		WSACleanup();
		success = false;
		return success;
	}

	activeSocket = &accountSocket;
	packetLog.open("packetDebugLog.txt");

	if (!recvThreadAcct) {
		recvThreadAcct = SDL_CreateThread(receivePacket, "AcctPacketReceiver", (void*)this);
		if (NULL == recvThreadAcct) {
			std::cout << "SDL_CreateThread failed: " << SDL_GetError() << "\n";
			success = false;
		}
	}

	socketDisconnected = false;
	return success;
}


bool Client::connectGameServer() {
	bool success = true;
	//load host dynamically from config
#ifndef LOCALSERVER
#ifndef DEVSERVER
	const char* host = "209.159.153.58";
#else
	const char* host = "127.0.0.1";
#endif // !DEVSERVER
#else
	const char* host = "127.0.0.1";
#endif // !LOCALSERVER

	WSADATA wsa;
	std::cout << "Initializing Winsock." << std::endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "Winsock Initialization failed: " << WSAGetLastError() << std::endl;
		success = false;
		return success;
	}

	if ((gameSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		std::cout << "Could not create socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		success = false;
		return success;
	}

	unsigned int gamePort = 9527;

	gameAddr.sin_family = AF_INET;
	gameAddr.sin_port = htons(gamePort);
	gameAddr.sin_addr.s_addr = inet_addr(host);

	// Make a connection to the server with socket SendingSocket.
	int retCode;
	retCode = connect(gameSocket, (SOCKADDR *)&gameAddr, sizeof(gameAddr));
	if (retCode != 0) {
		printf("Client: connect() failed! Error code: %ld\n", WSAGetLastError());
		// Close the socket
		closesocket(gameSocket);
		// Do the clean up
		WSACleanup();
		success = false;
		return success;
	}

	activeSocket = &gameSocket;
	socketDisconnected = false;
	checkGsConnection = true;
	return success;
}


void Client::shutdownSocket() {
	SOCKET* cleanupSocket = activeSocket;
	activeSocket = nullptr;
	
	if (!cleanupSocket) return;
	if (shutdown(*cleanupSocket, SD_SEND) != 0)
		printf("Client: Well, there is something wrong with the shutdown(). The error code : %ld\n", WSAGetLastError());
	else
		printf("Client: shutdown() looks OK...\n");

	// When you are finished sending and receiving data on socket SendingSocket,
	// you should close the socket using the closesocket API.
	if (closesocket(*cleanupSocket) != 0)
		printf("Client: Cannot close \"SendingSocket\" socket. Error code: %ld\n", WSAGetLastError());

	else
		printf("Client: Closing \"SendingSocket\" socket...\n");

	// When your application is finished handling the connection, call WSACleanup.
	if (WSACleanup() != 0)
		printf("Client: WSACleanup() failed!...\n");

	else
		printf("Client: WSACleanup() is OK...\n");
}


bool Client::isConnectionAlive() {
	if (!activeSocket) return false;
	
	int r = send(*activeSocket, NULL, 0, 0);
	//if (r == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET) {
	if (r == SOCKET_ERROR) {
		//client has disconnected!
		printf("The socket is disconnected.\n");
		return false;
	}
	return true;
}


bool Client::wasDisconnected() {
	bool sd = socketDisconnected;
	socketDisconnected = false;
	return sd;
}


void Client::initializeGUI() {
	if (gui) return;

	std::string skin = "Ferocious";
	gui = new GUI(renderer, skin);
}


int Client::asyncClient(void *ptr) {
	Client* client = (Client*)ptr;
	while (true) {
		if (client->closeThread) break;

		//Load Core Components
		if (client->loadWDF()) {
			client->loadINI();
		}

		//Check for packets to send
		client->sendPacket();

		//Load any assets pending in the queue
		assetManager.loadAssets();
		/*client->loadingSurface = client->getNextSurfaceFromQueue();
		if (!client->loadingSurface.empty()) {
			client->loadSurface(client->loadingSurface);
		}*/

		SDL_Delay(10);
	}
	return 0;
}


bool Client::loadWDF() {
	if (!wdfCharacter) {
		wdfCharacter = new WDF("character.wdf");
		wdfCharacter->load();
		return false;
	}

	if (!wdfData) {
		wdfData = new WDF("data.wdf");
		wdfData->load();
		return false;
	}

	if (!wdfPet) {
		wdfPet = new WDF("pet.wdf");
		wdfPet->load();
		return false;

	}
	
	if (wdfCharacter && !wdfCharacter->isSorted()) {
		wdfCharacter->sortList();
		return false;
	}

	if (wdfData && !wdfData->isSorted()) {
		wdfData->sortList();
		return false;
	}

	if (wdfPet && !wdfPet->isSorted()) {
		wdfPet->sortList();
		return false;
	}

	return true;
}


void Client::loadINI() {
	if (!npcRle) npcRle = new INI("ANI\\npcRle.ani");
	if (!shopIni) shopIni = new INI("INI\\shop.ini");
	if (!itemIni) itemIni = new INI("INI\\item.ini");
}


void Client::getFileFromWDF(std::string filePath, std::shared_ptr<DataBuffer> &buffer, int &size) {
	std::string rootDir;
	std::string searchFile = filePath;

	int rootDirPos = filePath.find('\\');
	if (rootDirPos == std::string::npos) rootDirPos = filePath.find('/');
	if (rootDirPos != std::string::npos) {
		rootDir = filePath.substr(0, rootDirPos);
		transform(rootDir.begin(), rootDir.end(), rootDir.begin(), ::tolower);
		searchFile = searchFile.substr(rootDirPos + 1, std::string::npos);
	}

	switch (stringToWdf(rootDir)) {
	case wCharacter:
		SDL_LockMutex(cwdfMutex);
		wdfCharacter->getFileData(searchFile, buffer, size);
		SDL_UnlockMutex(cwdfMutex);
		break;
	case wPet:
		SDL_LockMutex(pwdfMutex);
		wdfPet->getFileData(searchFile, buffer, size);
		SDL_UnlockMutex(pwdfMutex);
		break;
	case wData:
		SDL_LockMutex(dwdfMutex);
		wdfData->getFileData(searchFile, buffer, size);
		SDL_UnlockMutex(dwdfMutex);
		break;
	}
}


int Client::stringToWdf(std::string wdfFile) {
	if (wdfFile == "character") return wCharacter;
	if (wdfFile == "pet") return wPet;
	if (wdfFile == "data") return wData;
}


int Client::receivePacket(void *ptr) {
	Client* client = (Client*)ptr;
	while (true) {
		if (client->closeThread) break;
		if (client->activeSocket == nullptr || *client->activeSocket == 0) {
			continue;
		}

		char packetHeader[4];
		char decHeader[4];
		int recvBytes = 0;
		if (client->activeSocket) recvBytes = recv(*client->activeSocket, packetHeader, 4, 0);
		int temp = WSAGetLastError();
		if (recvBytes == 4) {
			client->decryptPacket((BYTE*)decHeader, (BYTE*)packetHeader, 4);

			int size = (decHeader[0] & 0xFF) | (decHeader[1] << 8);
			int type = (decHeader[2] & 0xFF) | (decHeader[3] << 8);

			int payloadSize = size - 4;
			char* buffer = new char[size - 4];
			if (client->activeSocket) recvBytes = recv(*client->activeSocket, buffer, payloadSize, 0);
			if (recvBytes == payloadSize) {
				client->createPacketByType(type, size, packetHeader, buffer);
			}
		}

		if (client->checkGsConnection && recvBytes <= 0) {
			client->socketDisconnected = true;
			client->shutdownSocket();
			printf("Game Server is disconnected!\n");
			continue;
		}
		SDL_Delay(10);
	}
	return 0;
}


void Client::sendPacket() {
	if (gameSocket == 0) return;

	std::vector<Packet*> sendPackets;

	SDL_LockMutex(sendMutex);
	while (sendQueue.size() > 0) {
		sendPackets.push_back(sendQueue.front());
		sendQueue.pop();
	}
	SDL_UnlockMutex(sendMutex);

	if (sendPackets.size() == 0) return;

	BYTE *sendBuffer;
	int bufferSize = 0;
	for (int i = 0; i < sendPackets.size(); i++) {
		Packet* nextPacket = sendPackets[i];
		bufferSize += nextPacket->getLength();
	}

	sendBuffer = new BYTE[bufferSize];
	int bufferPos = 0;
	for (int i = 0; i < sendPackets.size(); i++) {
		Packet* nextPacket = sendPackets[i];
		nextPacket->encrypt();
		char* pBuffer = (char*)nextPacket->getBuffer();
		for (int j = 0; j < nextPacket->getLength(); j++) {
			sendBuffer[bufferPos++] = pBuffer[j];
		}
	}

	int bytesSent = send(gameSocket, (char*)sendBuffer, bufferSize, 0);
	for (int i = 0; i < sendPackets.size(); i++) {
		Packet* nextPacket = sendPackets[i];
		if (bytesSent == SOCKET_ERROR)
			printf("Client: send() error %ld.\n", WSAGetLastError());
		else {
			//if (socketDebug) nextPacket->debugPrint();
		}
		delete nextPacket;
	}

	if (useSeedAfterSend) {
		decryptCipher.reset();
		useSeed = true;
		useSeedAfterSend = false;
	}
}


void Client::encryptPacket(BYTE* encryptedBuffer, BYTE* buffer, int len) {
	for (int i = 0; i < len; i++) {
		encryptedBuffer[i] = encryptCipher.encrypt(buffer[i], useSeed);
	}
}


void Client::decryptPacket(BYTE* decryptedBuffer, BYTE* buffer, int len) {
	for (int i = 0; i < len; i++) {
		decryptedBuffer[i] = decryptCipher.decrypt(buffer[i], false);
	}
}


void Client::addPacket(Packet* packet) {
	SDL_LockMutex(sendMutex);
	sendQueue.push(packet);
	SDL_UnlockMutex(sendMutex);
}


void Client::addPacket(std::vector<Packet*> packets) {
	SDL_LockMutex(sendMutex);
	for (auto packet : packets) {
		sendQueue.push(packet);
	}
	SDL_UnlockMutex(sendMutex);
}


void Client::createPacketByType(int type, int size, char* header, char* buffer) {
	char* fullBuffer = new char[size];
	memcpy(fullBuffer, header, 4);
	memcpy(fullBuffer+4, buffer, size-4);

	char* fullDecryptBuffer = new char[size];
	fullDecryptBuffer[0] = (size & 0xFF);
	fullDecryptBuffer[1] = (size >> 8);
	fullDecryptBuffer[2] = (type & 0xFF);
	fullDecryptBuffer[3] = (type >> 8);

	decryptPacket((BYTE*)fullDecryptBuffer+4, (BYTE*)buffer, size - 4);

	Packet* packet = nullptr;

	switch (type) {
	case ptMessage:
		packet = new pMessage(size, fullDecryptBuffer, fullBuffer);
		break;
	case ptWalk:
		packet = new pWalk(fullDecryptBuffer, fullBuffer);
		break;
	case ptEmotion:
		packet = new pAction(fullDecryptBuffer, fullBuffer);
		break;
	case ptAction:
		packet = new pAction(fullDecryptBuffer, fullBuffer);
		break;
	case ptDirection:
		packet = new pDirection(fullDecryptBuffer, fullBuffer);
		break;
	case ptUserInfo:
		packet = new pUserInfo(size, fullDecryptBuffer, fullBuffer);
		break;
	case ptRename:
		packet = new pRename(fullDecryptBuffer, fullBuffer);
		break;
	case ptRole:
		packet = new pRole(fullDecryptBuffer, fullBuffer);
		break;
	case ptLoginResponse:
		packet = new pLoginResponse(fullDecryptBuffer, fullBuffer);
		break;
	case ptGsConnect:
		packet = new pGsConnect(fullDecryptBuffer, fullBuffer);
		break;
	case ptPlayerInfo:
		packet = new pPlayerInfo(size, fullDecryptBuffer, fullBuffer);
		break;
	case ptItem:
		packet = new pItem(fullDecryptBuffer, fullBuffer);
		break;
	case ptItemAction:
		packet = new pItemAction(fullDecryptBuffer, fullBuffer);
		break;
	case ptTick:
		packet = new pTick(fullDecryptBuffer, fullBuffer);
		break;
	case ptMapInfo:
		packet = new pMapInfo(fullDecryptBuffer, fullBuffer);
		break;
	case ptPetAction:
		packet = new pPetAction(fullDecryptBuffer, fullBuffer);
		break;
	case ptPetInfo:
		packet = new pPetInfo(fullDecryptBuffer, fullBuffer);
		break;
	case ptUserAttribute:
		packet = new pUserAttribute(size, fullDecryptBuffer, fullBuffer);
		break;
	case ptColor:
		packet = new pColor(fullDecryptBuffer, fullBuffer);
		break;
	case ptBattleAction:
		packet = new pBattleAction(fullDecryptBuffer, fullBuffer);
		break;
	case ptNormalAct:
		packet = new pNormalAct(fullDecryptBuffer, fullBuffer);
		break;
	case ptItemAct:
		packet = new pItemAct(fullDecryptBuffer, fullBuffer);
		break;
	case ptBattleState:
		packet = new pBattleState(fullDecryptBuffer, fullBuffer);
		break;
	case ptFighterInfo:
		packet = new pFighterInfo(size, fullDecryptBuffer, fullBuffer);
		break;
	case ptEnemyInfo:
		packet = new pEnemyInfo(size, fullDecryptBuffer, fullBuffer);
		break;
	case ptBattleResult:
		packet = new pBattleResult(fullDecryptBuffer, fullBuffer);
		break;
	case ptUserLevelUp:
		packet = new pUserLevelUp(fullDecryptBuffer, fullBuffer);
		break;
	case ptPetLevelUp:
		packet = new pPetLevelUp(fullDecryptBuffer, fullBuffer);
		break;
	case ptBattleRound:
		packet = new pBattleRound(fullDecryptBuffer, fullBuffer);
		break;
	case ptFriend:
		packet = new pFriend(size, fullDecryptBuffer, fullBuffer);
		break;
	case ptNpcInfo:
		packet = new pNpcInfo(fullDecryptBuffer, fullBuffer);
		break;
	case ptNpcDialogue:
		packet = new pNpcDialogue(size, fullDecryptBuffer, fullBuffer);
		break;
	case ptColosseum:
		packet = new pColosseum(fullDecryptBuffer, fullBuffer);
		break;

	default:
		packet = new Packet(type, size, fullDecryptBuffer, fullBuffer);
	}

	if (!closeThread && packet) {
		SDL_LockMutex(packetMutex);
		if (socketDebug) packet->debugPrint();
		receiveQueue.push(packet);
		SDL_UnlockMutex(packetMutex);
	}
}


void Client::logPacketError(std::string error) {
	packetLog << error << "\n";
}

void Client::writePacketToLog(Packet* packet) {
	packetLog << std::dec;
	packetLog << packet->getDescription() << " - Type: " << packet->getType() << " Size: " << packet->getLength() << std::endl;
	BYTE* buf = packet->getDecryptedBuffer();
	if (buf) {
		//Skip first four bytes, type and size
		for (int i = 4; i < packet->getLength(); i++)
		{
			packetLog << std::setw(2) << std::setfill('0') << std::hex << (int)(unsigned char)buf[i];
			//packetLog << buf[i];
		}
		packetLog << "\n";
		for (int i = 4; i < packet->getLength(); i++)
		{
			//packetLog << (int)(unsigned char)buf[i];
			packetLog << buf[i];
		}
	}
	packetLog << "\n\n";
}

void Client::login(std::string user, std::string password, std::string server) {
	//connectAccountServer();
	strServer = server;
	pLoginRequest* packet = new pLoginRequest((char*)user.c_str(), (char*)password.c_str(), (char*)server.c_str());
	
	packet->encrypt();
	encryptCipher.reset();
	//decryptCipher.reset();

	int bytesSent = send(accountSocket, (char*)packet->getBuffer(), packet->getLength(), 0);
	if (bytesSent == SOCKET_ERROR)
		printf("Client: send() error %ld.\n", WSAGetLastError());
	else {
		if (socketDebug) packet->debugPrint();
	}
}


void Client::handleLoginResponse(pLoginResponse* packet) {
	loginResponseReceived = true;

	accountId = packet->accountID;
	loginSeed = packet->seed;
	loginMessage = (char*)packet->phrase;

	encryptCipher.seed(packet->seed);
	encryptCipher.reset();
	decryptCipher.reset();
	useSeedAfterSend = true;
}


bool Client::getLoginResponse(int &account, int &seed, std::string &message) {
	if (!loginResponseReceived) return false;
	account = accountId;
	seed = loginSeed;
	message = loginMessage;
	return true;
}


void Client::clearLoginResponse() {
	loginResponseReceived = false;
	accountId = 0;
	loginSeed = 0;
	loginMessage.clear();

	encryptCipher.reset();
	decryptCipher.reset();
	useSeedAfterSend = false;
}


void Client::handlePackets() {
	Packet* packet = nullptr;

	//int count = 0; //temp test for packet loading limiter. one of the map loading subpackets is causing a bad slowdown
	do {
		//if (count > 5) break;
		if (packet) {
			delete packet;
			packet = nullptr;
		}
		packet = getNextPacket();
		if (packet) {
			int start = SDL_GetTicks();
			packet->process();
			int elapsed = SDL_GetTicks() - start;
			if (elapsed > 10) 	std::cout << "**********" << packet->getDescription() << "->process() ticks elapsed: " << elapsed << std::endl;
		}
		//count++;
	} while (packet);
}


Packet* Client::getNextPacket() {
	Packet* packet = nullptr;

	SDL_LockMutex(packetMutex);

	if (receiveQueue.size() > 0) {
		packet = receiveQueue.front();
		receiveQueue.pop();
	}

	SDL_UnlockMutex(packetMutex);

	return packet;
}

void Client::addToDebugLog(std::string message) {
#ifdef DEBUG_LOG
	SDL_LockMutex(dbgMutex);
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		debugLog << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << " " << message << std::endl;
	SDL_UnlockMutex(dbgMutex);
#endif
}