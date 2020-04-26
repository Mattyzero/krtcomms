#pragma once
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_errors.h"
#include <QtCore/QMap>
#include <QtCore/QList>

class Talkers
{
public:
	static Talkers& getInstance();

	struct ClientInfo {
		anyID clientID;
		bool isWhispering;
		int frequence;
	};

	Talkers();
	~Talkers();


	void Add(uint64 serverConnectionHandlerID, anyID clientID, int isReceivedWhisper, int frequence);
	void Remove(uint64 serverConnectionHandlerID, anyID clientID, int isReceivedWhisper);

	bool IsWhispering(uint64 serverConnectionHandlerID, anyID clientID);
	bool IsAnyWhispering(uint64 serverConnectionHandlerID);
	bool PrioritizedFrequence(uint64 serverConnectionHandlerID, anyID clientID);

private:
	QMap<uint64, QMap<anyID, ClientInfo*>> _talkers;
};

